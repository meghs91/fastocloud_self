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

#include "stream_commands/commands_factory.h"

#include "stream_commands/commands.h"

namespace fastocloud {

fastotv::protocol::response_t RestartStreamResponseSuccess(fastotv::protocol::sequance_id_t id) {
  return fastotv::protocol::response_t::MakeMessage(id,
                                                    common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
}

fastotv::protocol::response_t StopStreamResponseSuccess(fastotv::protocol::sequance_id_t id) {
  return fastotv::protocol::response_t::MakeMessage(id,
                                                    common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
}

fastotv::protocol::request_t RestartStreamRequest(fastotv::protocol::sequance_id_t id) {
  fastotv::protocol::request_t req;
  req.id = id;
  req.method = RESTART_STREAM;
  return req;
}

fastotv::protocol::request_t StopStreamRequest(fastotv::protocol::sequance_id_t id) {
  fastotv::protocol::request_t req;
  req.id = id;
  req.method = STOP_STREAM;
  return req;
}

}  // namespace fastocloud
