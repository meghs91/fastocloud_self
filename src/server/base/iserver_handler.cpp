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

#include "server/base/iserver_handler.h"

namespace fastocloud {
namespace server {
namespace base {

IServerHandler::IServerHandler() : online_clients_(0) {}

size_t IServerHandler::GetOnlineClients() const {
  return online_clients_;
}

void IServerHandler::Accepted(common::libev::IoClient* client) {
  UNUSED(client);
  online_clients_++;
}

void IServerHandler::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
  online_clients_--;
}

void IServerHandler::Closed(common::libev::IoClient* client) {
  UNUSED(client);
  online_clients_--;
}

}  // namespace base
}  // namespace server
}  // namespace fastocloud
