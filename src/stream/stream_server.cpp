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

#include "stream/stream_server.h"

#include "stream/commands_factory.h"
#include "stream_commands/commands.h"

namespace fastocloud {
namespace stream {

StreamServer::StreamServer(fastotv::protocol::protocol_client_t* command_client,
                           common::libev::IoLoopObserver* observer)
    : base_class(new common::libev::LibEvLoop, observer), command_client_(command_client) {
  CHECK(command_client);
}

void StreamServer::WriteRequest(const fastotv::protocol::request_t& request) {
  auto cb = [this, request] { ignore_result(command_client_->WriteRequest(request)); };
  ExecInLoopThread(cb);
}

const char* StreamServer::ClassName() const {
  return "StreamServer";
}

void StreamServer::SendChangeSourcesBroadcast(const ChangedSouresInfo& change) {
  fastotv::protocol::request_t req;
  common::Error err = ChangedSourcesStreamBroadcast(change, &req);
  if (err) {
    return;
  }

  WriteRequest(req);
}

void StreamServer::SendStatisticBroadcast(const StatisticInfo& statistic) {
  fastotv::protocol::request_t req;
  common::Error err = StatisticStreamBroadcast(statistic, &req);
  if (err) {
    return;
  }

  WriteRequest(req);
}

bool StreamServer::IsCanBeRegistered(common::libev::IoClient* client) const {
  return client == command_client_;
}

#if defined(MACHINE_LEARNING)
void StreamServer::SendMlNotificationBroadcast(const fastotv::commands_info::ml::NotificationInfo& notification) {
  fastotv::protocol::request_t req;
  common::Error err = NotificationMlStreamBroadcast(notification, &req);
  if (err) {
    return;
  }

  WriteRequest(req);
}
#endif

common::libev::IoChild* StreamServer::CreateChild() {
  NOTREACHED();
  return nullptr;
}

common::libev::IoClient* StreamServer::CreateClient(const common::net::socket_info& info) {
  UNUSED(info);
  NOTREACHED();
  return nullptr;
}

void StreamServer::Started(common::libev::LibEvLoop* loop) {
  CHECK(RegisterClient(command_client_)) << "Must be for communication!";
  base_class::Started(loop);
}

void StreamServer::Stopped(common::libev::LibEvLoop* loop) {
  UnRegisterClient(command_client_);
  base_class::Stopped(loop);
}

}  // namespace stream
}  // namespace fastocloud
