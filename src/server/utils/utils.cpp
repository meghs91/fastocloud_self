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

#include "server/utils/utils.h"

#include <unistd.h>

namespace {
#if defined(OS_WIN)
int socketpair(int domain, int type, int protocol, SOCKET socks[2]) {
  SOCKET listener = socket(domain, type, protocol);
  if (listener == INVALID_SOCKET_VALUE) {
    return SOCKET_ERROR;
  }

  struct sockaddr_in addr = {0};
  addr.sin_family = domain;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  socklen_t addr_size = sizeof(addr);

  struct sockaddr_in adr2;
  socklen_t adr2_size = sizeof(adr2);
  if (bind(listener, (struct sockaddr*)&addr, addr_size) == SOCKET_ERROR) {
    goto error;
  }
  if (getsockname(listener, (struct sockaddr*)&addr, &addr_size) == SOCKET_ERROR) {
    goto error;
  }
  if (listen(listener, 1) == SOCKET_ERROR) {
    goto error;
  }

  socks[0] = socket(domain, type, protocol);
  if (socks[0] == INVALID_SOCKET_VALUE) {
    goto error;
  }

  if (connect(socks[0], (struct sockaddr*)&addr, addr_size) == SOCKET_ERROR) {
    goto error;
  }

  socks[1] = accept(listener, nullptr, nullptr);
  if (socks[1] == INVALID_SOCKET_VALUE) {
    goto error;
  }

  if (getpeername(socks[0], (struct sockaddr*)&addr, &addr_size)) {
    goto error;
  }

  if (getsockname(socks[1], (struct sockaddr*)&adr2, &adr2_size)) {
    goto error;
  }

  closesocket(listener);
  return 0;

error:
  closesocket(listener);
  closesocket(socks[0]);
  closesocket(socks[1]);
  return ERROR_RESULT_VALUE;
}
#endif
}  // namespace

namespace fastocloud {
namespace server {

#if defined(OS_POSIX)
common::ErrnoError CreatePipe(common::net::socket_descr_t* read_client_fd,
                              common::net::socket_descr_t* write_client_fd) {
  if (!read_client_fd || !write_client_fd) {
    return common::make_errno_error_inval();
  }

  int pipefd[2] = {INVALID_DESCRIPTOR, INVALID_DESCRIPTOR};
  int res = pipe(pipefd);
  if (res == ERROR_RESULT_VALUE) {
    return common::make_errno_error(errno);
  }

  *read_client_fd = pipefd[0];
  *write_client_fd = pipefd[1];
  return common::ErrnoError();
}
#endif

common::ErrnoError CreateSocketPair(common::net::socket_descr_t* parent_sock, common::net::socket_descr_t* child_sock) {
  if (!parent_sock || !child_sock) {
    return common::make_errno_error_inval();
  }

  common::net::socket_descr_t socks[2];
#if defined(OS_POSIX)
  int res = socketpair(AF_LOCAL, SOCK_STREAM, 0, socks);
#else
  int res = socketpair(AF_INET, SOCK_STREAM, 0, socks);
#endif
  if (res == ERROR_RESULT_VALUE) {
    return common::make_errno_error(errno);
  }

  *parent_sock = socks[1];
  *child_sock = socks[0];
  return common::ErrnoError();
}

}  // namespace server
}  // namespace fastocloud
