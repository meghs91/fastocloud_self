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

#include <common/libev/pipe_client.h>

#include <fastotv/protocol/protocol.h>

namespace fastocloud {
namespace server {
namespace pipe {

class Client : public fastotv::protocol::protocol_client_t {
 public:
  typedef fastotv::protocol::ProtocolClient<common::libev::IoClient> base_class;
  ~Client() override;

  const char* ClassName() const override;

  Client(common::libev::IoLoop* server, descriptor_t read_fd, descriptor_t write_fd);

 protected:
  descriptor_t GetFd() const override;

 private:
  common::ErrnoError DoSingleWrite(const void* data, size_t size, size_t* nwrite_out) override;
  common::ErrnoError DoSingleRead(void* out, size_t max_size, size_t* nread) override;
  common::ErrnoError DoClose() override;

  common::libev::PipeReadClient* pipe_read_client_;
  common::libev::PipeWriteClient* pipe_write_client_;
  const descriptor_t read_fd_;

  DISALLOW_COPY_AND_ASSIGN(Client);
};

}  // namespace pipe
}  // namespace server
}  // namespace fastocloud
