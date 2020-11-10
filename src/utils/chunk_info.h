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

namespace fastocloud {
namespace utils {

struct ChunkInfo {
  enum { SECOND = 1000000000 };

  ChunkInfo();
  ChunkInfo(const std::string& path, uint64_t duration, uint64_t index);

  double GetDurationInSecconds() const;

  std::string path;
  uint64_t duration;  // in nanoseconds
  uint64_t index;
};

inline bool operator<(const ChunkInfo& left, const ChunkInfo& right) {
  if (left.duration != right.duration) {
    return left.duration < right.duration;
  }

  return left.index < right.index;
}

inline bool operator>(const ChunkInfo& left, const ChunkInfo& right) {
  if (left.duration != right.duration) {
    return left.duration > right.duration;
  }

  return left.index > right.index;
}

}  // namespace utils
}  // namespace fastocloud

namespace common {
std::string ConvertToString(const fastocloud::utils::ChunkInfo& value);
bool ConvertFromString(const std::string& from, fastocloud::utils::ChunkInfo* out);
}  // namespace common
