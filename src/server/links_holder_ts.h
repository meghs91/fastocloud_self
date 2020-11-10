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

#include <map>
#include <mutex>

#include "base/stream_config.h"

namespace fastocloud {
namespace server {

class LinksHolderTS {
 public:
  StreamConfig Find(const common::file_system::ascii_directory_string_path& path);
  void Insert(const common::file_system::ascii_directory_string_path& path, StreamConfig config);
  void Clear();

  std::map<common::file_system::ascii_directory_string_path, StreamConfig> Copy();

 private:
  std::mutex mutex_;
  std::map<common::file_system::ascii_directory_string_path, StreamConfig> links_;
};

}  // namespace server
}  // namespace fastocloud
