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

#include "server/daemon/commands_info/stream/start_info.h"

#include "base/stream_config_parse.h"

#define START_INFO_CONFIG_FIELD "config"

namespace fastocloud {
namespace server {
namespace stream {

StartInfo::StartInfo() : base_class(), config_() {}

StartInfo::StartInfo(const config_t& config) : config_(config) {}

StartInfo::config_t StartInfo::GetConfig() const {
  return config_;
}

common::Error StartInfo::DoDeSerialize(json_object* serialized) {
  if (!serialized) {
    return common::make_error_inval();
  }

  json_object* jconfig;
  common::Error err = GetObjectField(serialized, START_INFO_CONFIG_FIELD, &jconfig);
  if (err) {
    return err;
  }

  config_t conf = MakeConfigFromJson(jconfig);
  if (!conf) {
    return common::make_error_inval();
  }

  *this = StartInfo(conf);
  return common::Error();
}

common::Error StartInfo::SerializeFields(json_object*) const {
  if (!config_) {
    return common::make_error_inval();
  }

  NOTREACHED() << "Not need";
  return common::Error();
}

}  // namespace stream
}  // namespace server
}  // namespace fastocloud
