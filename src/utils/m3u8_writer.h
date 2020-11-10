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

#include <common/file_system/file.h>

namespace fastocloud {
namespace utils {

struct ChunkInfo;

class M3u8Writer {
 public:
  M3u8Writer();

  common::ErrnoError Open(const common::file_system::ascii_file_string_path& file_path,
                          uint32_t flags) WARN_UNUSED_RESULT;

  common::ErrnoError WriteHeader(uint64_t first_index, size_t target_duration) WARN_UNUSED_RESULT;
  common::ErrnoError WriteLine(const ChunkInfo& chunks) WARN_UNUSED_RESULT;
  common::ErrnoError WriteFooter() WARN_UNUSED_RESULT;
  common::ErrnoError Close() WARN_UNUSED_RESULT;

 private:
  common::file_system::File file_;
};

}  // namespace utils
}  // namespace fastocloud
