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

#pragma once

#include <limits>
#include <string>

#include <common/file_system/path.h>

namespace fastocloud {
namespace stream {

typedef uint64_t chunk_index_t;
static const chunk_index_t invalid_chunk_index = std::numeric_limits<chunk_index_t>::max();

typedef time_t chunk_life_time_t;
typedef time_t time_shift_delay_t;

struct TimeShiftInfo {
  TimeShiftInfo();
  explicit TimeShiftInfo(const std::string& path, chunk_life_time_t lth, time_shift_delay_t delay);

  bool FindLastChunk(chunk_index_t* index, time_t* file_created_time) const WARN_UNUSED_RESULT;
  bool FindChunkToPlay(time_t chunk_duration, chunk_index_t* index) const WARN_UNUSED_RESULT;

  common::file_system::ascii_directory_string_path timshift_dir;
  chunk_life_time_t timeshift_chunk_life_time;
  time_shift_delay_t timeshift_delay;
};

}  // namespace stream
}  // namespace fastocloud
