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

#include <map>

#include "server/links_holder_ts.h"

namespace fastocloud {
namespace server {

StreamConfig LinksHolderTS::Find(const common::file_system::ascii_directory_string_path& path) {
  std::unique_lock<std::mutex> lock(mutex_);
  auto it = links_.find(path);
  if (it == links_.end()) {
    return StreamConfig();
  }

  return it->second;
}

void LinksHolderTS::Insert(const common::file_system::ascii_directory_string_path& path, StreamConfig config) {
  std::unique_lock<std::mutex> lock(mutex_);
  links_[path] = config;
}

void LinksHolderTS::Clear() {
  std::unique_lock<std::mutex> lock(mutex_);
  links_.clear();
}

std::map<common::file_system::ascii_directory_string_path, StreamConfig> LinksHolderTS::Copy() {
  std::unique_lock<std::mutex> lock(mutex_);
  return links_;
}

}  // namespace server
}  // namespace fastocloud
