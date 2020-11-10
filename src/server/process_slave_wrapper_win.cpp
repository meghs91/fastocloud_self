/*  Copyright (C) 2014-2019 FastoGT. All right reserved.
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

#include "server/process_slave_wrapper.h"

#include <common/libev/io_loop.h>

#include "base/stream_config_parse.h"

#include "server/child_stream.h"
#include "server/tcp/client.h"
#include "server/utils/utils.h"

namespace fastocloud {
namespace server {

common::ErrnoError ProcessSlaveWrapper::CreateChildStreamImpl(const serialized_stream_t& config_args,
                                                              const StreamInfo& sha) {
  const fastotv::stream_id_t sid = GetSid(config_args);
  common::net::socket_descr_t parent_sock;
  common::net::socket_descr_t child_sock;
  common::ErrnoError err = CreateSocketPair(&parent_sock, &child_sock);
  if (err) {
    return err;
  }

  SECURITY_ATTRIBUTES sa;
  memset(&sa, 0, sizeof(sa));
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;
  std::string json;
  if (!MakeJsonFromConfig(config_args, &json)) {
    return common::make_errno_error(EINTR);
  }

  const size_t proto_info_len = sizeof(WSAPROTOCOL_INFO);
  const size_t allocate_memory = proto_info_len + json.size();
  HANDLE args_handle = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, allocate_memory, nullptr);
  if (args_handle == INVALID_HANDLE_VALUE) {
    return common::make_errno_error(errno);
  }

  char* param = static_cast<char*>(MapViewOfFile(args_handle, FILE_MAP_WRITE, 0, 0, allocate_memory));
  if (!param) {
    CloseHandle(args_handle);
    return common::make_errno_error(errno);
  }

#define CMD_LINE_SIZE 512
  char cmd_line[CMD_LINE_SIZE] = {0};
#if defined(_WIN64)
  common::SNPrintf(cmd_line, CMD_LINE_SIZE, STREAMER_EXE_NAME ".exe %llu %llu", reinterpret_cast<UINT_PTR>(args_handle),
                   allocate_memory);
#else
  common::SNPrintf(cmd_line, CMD_LINE_SIZE, STREAMER_EXE_NAME ".exe %lu %lu", reinterpret_cast<DWORD>(args_handle),
                   allocate_memory);
#endif

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));
  memset(&si, 0, sizeof(si));
  if (!CreateProcess(nullptr, cmd_line, nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) {
    UnmapViewOfFile(param);
    CloseHandle(args_handle);
    return common::make_errno_error(errno);
  }

  WSAPROTOCOL_INFO pin;
  if (WSADuplicateSocket(child_sock, pi.dwProcessId, &pin) != 0) {
    UnmapViewOfFile(param);
    CloseHandle(args_handle);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return common::make_errno_error(errno);
  }

  closesocket(child_sock);
  memcpy(param, &pin, proto_info_len);
  memcpy(param + proto_info_len, json.c_str(), json.size());

  UnmapViewOfFile(param);
  CloseHandle(args_handle);

  if (ResumeThread(pi.hThread) == -1) {
    if (!TerminateProcess(pi.hProcess, EXIT_FAILURE)) {
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return common::make_errno_error(errno);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return common::make_errno_error(errno);
  }

  tcp::Client* sock_client = new tcp::Client(loop_, common::net::socket_info(parent_sock));
  sock_client->SetName(sid);
  bool registered = loop_->RegisterClient(sock_client);
  if (!registered) {
    return common::make_errno_error("Can't register communication pipe", EAGAIN);
  }
  ChildStream* child = new ChildStream(loop_, sha);
  child->SetClient(sock_client);
  loop_->RegisterChild(child, pi.hProcess);
  CloseHandle(pi.hThread);
  return common::ErrnoError();
}

}  // namespace server
}  // namespace fastocloud
