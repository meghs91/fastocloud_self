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

#include "server/pipe/client.h"

namespace fastocloud {
namespace server {
namespace pipe {

Client::Client(common::libev::IoLoop* server, descriptor_t read_fd, descriptor_t write_fd)
    : base_class(std::make_shared<fastotv::protocol::FastoTVCompressor>(), server),
      pipe_read_client_(new common::libev::PipeReadClient(nullptr, read_fd)),
      pipe_write_client_(new common::libev::PipeWriteClient(nullptr, write_fd)),
      read_fd_(read_fd) {}

common::ErrnoError Client::DoSingleWrite(const void* data, size_t size, size_t* nwrite_out) {
  return pipe_write_client_->SingleWrite(data, size, nwrite_out);
}

common::ErrnoError Client::DoSingleRead(void* out, size_t max_size, size_t* nread) {
  return pipe_read_client_->SingleRead(out, max_size, nread);
}

descriptor_t Client::GetFd() const {
  return read_fd_;
}

common::ErrnoError Client::DoClose() {
  ignore_result(pipe_write_client_->Close());
  ignore_result(pipe_read_client_->Close());
  return common::ErrnoError();
}

const char* Client::ClassName() const {
  return "PipeClient";
}

Client::~Client() {
  destroy(&pipe_write_client_);
  destroy(&pipe_read_client_);
}

}  // namespace pipe
}  // namespace server
}  // namespace fastocloud
