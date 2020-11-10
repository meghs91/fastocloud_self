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

#include "server/daemon/commands_info/stream/stream_info.h"

namespace fastocloud {
namespace server {
namespace stream {

class StopInfo : public StreamInfo {
 public:
  typedef StreamInfo base_class;
  StopInfo();
  explicit StopInfo(fastotv::stream_id_t stream_id);
};

}  // namespace stream
}  // namespace server
}  // namespace fastocloud
