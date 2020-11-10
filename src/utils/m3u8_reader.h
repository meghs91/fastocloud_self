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

#include <string>
#include <vector>

#include <common/file_system/path.h>

#include "utils/chunk_info.h"

namespace fastocloud {
namespace utils {

class M3u8Reader {
 public:
  M3u8Reader();

  bool Parse(const std::string& path);
  bool Parse(const common::file_system::ascii_file_string_path& path);

  int GetVersion() const;
  bool IsAllowCache() const;
  int GetMediaSequence() const;
  int GetTargetDuration() const;
  std::vector<ChunkInfo> GetChunks() const;

 private:
  void Clear();

  bool ParseFile(FILE* file);
  bool ParseChunks(FILE* file);

  int version_;
  bool allow_cache_;
  int media_sequence_;
  int target_duration_;

  std::vector<ChunkInfo> chunks_;
};

}  // namespace utils
}  // namespace fastocloud
