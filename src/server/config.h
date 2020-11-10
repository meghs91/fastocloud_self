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

#include <common/error.h>
#include <common/license/types.h>
#include <common/net/types.h>
#include <common/optional.h>

namespace fastocloud {
namespace server {

struct Config {
  typedef common::Optional<common::license::expire_key_t> license_t;

  Config();

  static common::net::HostAndPort GetDefaultHost();

  bool IsValid() const;

  common::net::HostAndPort host;
  std::string log_path;
  common::logging::LOG_LEVEL log_level;
  common::net::HostAndPort http_host;
  common::net::HostAndPort vods_host;
  common::net::HostAndPort cods_host;
  time_t cods_ttl;  // in seconds
  time_t files_ttl;
  std::string streamlink_path;
  license_t license_key;
};

common::ErrnoError load_config_from_file(const std::string& config_absolute_path, Config* config) WARN_UNUSED_RESULT;

}  // namespace server
}  // namespace fastocloud
