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

#include "server/tcp/client.h"

namespace fastocloud {
namespace server {
namespace tcp {

Client::Client(common::libev::IoLoop* server, const common::net::socket_info& info)
    : base_class(std::make_shared<fastotv::protocol::FastoTVCompressor>(), server),
      client_(new common::libev::tcp::TcpClient(nullptr, info)) {}

const char* Client::ClassName() const {
  return "TcpClient";
}

common::ErrnoError Client::DoSingleWrite(const void* data, size_t size, size_t* nwrite_out) {
  return client_->SingleWrite(data, size, nwrite_out);
}

common::ErrnoError Client::DoSingleRead(void* out, size_t max_size, size_t* nread) {
  return client_->SingleRead(out, max_size, nread);
}

descriptor_t Client::GetFd() const {
  return client_->GetInfo().fd();
}

common::ErrnoError Client::DoClose() {
  return client_->Close();
}

}  // namespace tcp
}  // namespace server
}  // namespace fastocloud
