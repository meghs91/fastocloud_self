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

#include "server/daemon/commands_info/service/sync_info.h"

#include "base/stream_config_parse.h"

#define SYNC_INFO_STREAMS_FIELD "streams"

namespace fastocloud {
namespace server {
namespace service {

SyncInfo::SyncInfo() : base_class(), streams_() {}

SyncInfo::SyncInfo(const streams_t& streams) : base_class(), streams_(streams) {}

SyncInfo::streams_t SyncInfo::GetStreams() const {
  return streams_;
}

common::Error SyncInfo::SerializeFields(json_object*) const {
  NOTREACHED() << "Not need";
  return common::Error();
}

common::Error SyncInfo::DoDeSerialize(json_object* serialized) {
  json_object* jstreams;
  size_t len;
  common::Error err = GetArrayField(serialized, SYNC_INFO_STREAMS_FIELD, &jstreams, &len);
  streams_t streams;
  if (!err) {
    for (size_t i = 0; i < len; ++i) {
      json_object* jstream = json_object_array_get_idx(jstreams, i);
      config_t conf = MakeConfigFromJson(jstream);
      if (conf) {
        streams.push_back(conf);
      }
    }
  }

  *this = SyncInfo(streams);
  return common::Error();
}

}  // namespace service
}  // namespace server
}  // namespace fastocloud
