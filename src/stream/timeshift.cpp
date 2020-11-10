/*  Copyright (C) 2014-2020 FastoGT. All right reserved.
    This file is part of fastocloud.
    fastocloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    fastocloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with fastocloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stream/timeshift.h"

#include <algorithm>
#include <string>

#include <common/convert2string.h>
#include <common/time.h>

#include <common/file_system/file_system.h>
#include <common/file_system/file_system_utils.h>
#include <common/file_system/string_path_utils.h>

#include "base/constants.h"
#include "stream/stypes.h"

namespace fastocloud {
namespace stream {

namespace {
bool compare_files(const common::file_system::ascii_file_string_path& first,
                   const common::file_system::ascii_file_string_path& second) {  // should be 0.ts, 1.ts, 2.ts
  std::string first_chunk = first.GetBaseFileName();
  std::string second_chunk = second.GetBaseFileName();

  chunk_index_t first_index;
  bool ok = common::ConvertFromString(first_chunk, &first_index);
  CHECK(ok) << "Must be index but: " << first_chunk;
  chunk_index_t second_index;
  ok = common::ConvertFromString(second_chunk, &second_index);
  CHECK(ok) << "Must be index but: " << second_chunk;
  return first_index < second_index;
}
}  // namespace

TimeShiftInfo::TimeShiftInfo()
    : timshift_dir(), timeshift_chunk_life_time(DEFAULT_CHUNK_LIFE_TIME), timeshift_delay(0) {}

TimeShiftInfo::TimeShiftInfo(const std::string& path, chunk_life_time_t lth, time_shift_delay_t delay)
    : timshift_dir(path), timeshift_chunk_life_time(lth), timeshift_delay(delay) {}

bool TimeShiftInfo::FindChunkToPlay(time_t chunk_duration, chunk_index_t* index) const {
  if (!index) {
    return false;
  }

  time_t desired_time = common::time::current_utc_mstime() / 1000 - timeshift_delay * 60;  // OK
  std::string absolute_path = timshift_dir.GetPath();
  if (!common::file_system::is_directory_exist(absolute_path)) {
    CRITICAL_LOG() << "Folder with chunks doesn't exist: " << absolute_path;
  }

  const std::function<bool(const common::file_system::ascii_file_string_path&)> filter =
      [](const common::file_system::ascii_file_string_path& path) {
        std::string file_name = path.GetBaseFileName();
        chunk_index_t index;
        return common::ConvertFromString(file_name, &index);
      };

  auto files = common::file_system::ScanFolder(timshift_dir, CHUNK_EXT, false, filter);
  if (files.empty()) {
    return false;
  }

  std::sort(files.begin(), files.end(), compare_files);
  chunk_index_t prev_index = invalid_chunk_index;
  for (size_t i = 0; i < files.size(); ++i) {
    std::string fname = files[i].GetBaseFileName();
    chunk_index_t lindex;
    bool ok = common::ConvertFromString(fname, &lindex);
    UNUSED(ok);
    time_t file_created_time;
    common::file_system::get_file_time_last_modification(files[i].GetPath(), &file_created_time);
    time_t diff = (desired_time - file_created_time) + chunk_duration;
    if (-chunk_duration < diff && diff < chunk_duration) {
      if (diff > 0 && prev_index != invalid_chunk_index) {
        *index = prev_index;
      } else {
        *index = lindex;
      }

      INFO_LOG() << "Select " << *index << " part, diff sec " << diff;
      return true;
    }

    prev_index = lindex;
  }

  return false;
}

bool TimeShiftInfo::FindLastChunk(chunk_index_t* index, time_t* file_created_time) const {
  if (!index || !file_created_time) {
    return false;
  }

  const std::string absolute_path = timshift_dir.GetPath();
  if (!common::file_system::is_directory_exist(absolute_path)) {
    CRITICAL_LOG() << "Folder with chunks doesn't exist: " << absolute_path;
  }

  const std::function<bool(const common::file_system::ascii_file_string_path&)> filter =
      [](const common::file_system::ascii_file_string_path& path) {
        std::string file_name = path.GetBaseFileName();
        chunk_index_t index;
        return common::ConvertFromString(file_name, &index);
      };

  auto files = common::file_system::ScanFolder(timshift_dir, CHUNK_EXT, false, filter);
  if (files.empty()) {
    return false;
  }

  std::sort(files.begin(), files.end(), compare_files);
  common::file_system::ascii_file_string_path last_file = files.back();
  common::ErrnoError err = common::file_system::get_file_time_last_modification(last_file.GetPath(), file_created_time);
  if (err) {
    return false;
  }
  chunk_index_t lindex;
  if (!common::ConvertFromString(last_file.GetBaseFileName(), &lindex)) {
    return false;
  }
  *index = lindex;
  return true;
}

}  // namespace stream
}  // namespace fastocloud
