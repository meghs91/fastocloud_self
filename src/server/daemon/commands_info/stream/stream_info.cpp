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

#include "server/daemon/commands_info/stream/stream_info.h"

#define STREAM_INFO_ID_FIELD "id"

namespace fastocloud {
namespace server {
namespace stream {

StreamInfo::StreamInfo() : base_class(), stream_id_() {}

StreamInfo::StreamInfo(const fastotv::stream_id_t& stream_id) : stream_id_(stream_id) {}

fastotv::stream_id_t StreamInfo::GetStreamID() const {
  return stream_id_;
}

common::Error StreamInfo::DoDeSerialize(json_object* serialized) {
  fastotv::stream_id_t sid;
  common::Error err = GetStringField(serialized, STREAM_INFO_ID_FIELD, &sid);
  if (err) {
    return err;
  }

  *this = StreamInfo(sid);
  return common::Error();
}

common::Error StreamInfo::SerializeFields(json_object* out) const {
  ignore_result(SetStringField(out, STREAM_INFO_ID_FIELD, stream_id_));
  return common::Error();
}

}  // namespace stream
}  // namespace server
}  // namespace fastocloud
