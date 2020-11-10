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

#include "server/process_slave_wrapper.h"

#if defined(OS_LINUX)
#include <sys/prctl.h>
#endif

#include <dlfcn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>

#include "base/stream_info.h"

#include "server/child_stream.h"
#include "server/daemon/server.h"
#include "server/utils/utils.h"

#include "stream/stream_wrapper.h"

#define PIPE 1

#if PIPE
#include "pipe/client.h"
#else
#include "tcp/client.h"
#endif

namespace fastocloud {
namespace server {

common::ErrnoError ProcessSlaveWrapper::CreateChildStreamImpl(const serialized_stream_t& config_args,
                                                              const StreamInfo& sha) {
  const fastotv::stream_id_t sid = GetSid(config_args);
#if PIPE
  common::net::socket_descr_t read_command_client;
  common::net::socket_descr_t write_requests_client;
  common::ErrnoError err = CreatePipe(&read_command_client, &write_requests_client);
  if (err) {
    return err;
  }

  common::net::socket_descr_t read_responce_client;
  common::net::socket_descr_t write_responce_client;
  err = CreatePipe(&read_responce_client, &write_responce_client);
  if (err) {
    common::ErrnoError errn = common::file_system::close_descriptor(read_command_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    errn = common::file_system::close_descriptor(write_requests_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    return err;
  }
#else
  common::net::socket_descr_t parent_sock;
  common::net::socket_descr_t child_sock;
  common::ErrnoError err = CreateSocketPair(&parent_sock, &child_sock);
  if (err) {
    return err;
  }
#endif

#if !defined(TEST)
  pid_t pid = fork();
#else
  pid_t pid = 0;
#endif
  if (pid == 0) {  // child
    typedef int (*stream_exec_t)(const char* process_name, const void* args, void* command_client);

    const std::string absolute_source_dir = common::file_system::absolute_path_from_relative(RELATIVE_SOURCE_DIR);
    const std::string lib_full_path = common::file_system::make_path(absolute_source_dir, CORE_LIBRARY);
    void* handle = dlopen(lib_full_path.c_str(), RTLD_LAZY);
    if (!handle) {
      ERROR_LOG() << "Failed to load " CORE_LIBRARY " path: " << lib_full_path << ", error: " << dlerror();
      _exit(EXIT_FAILURE);
    }

    stream_exec_t stream_exec_func = reinterpret_cast<stream_exec_t>(dlsym(handle, "stream_exec"));
    char* error = dlerror();
    if (error) {
      ERROR_LOG() << "Failed to load start stream function error: " << error;
      dlclose(handle);
      _exit(EXIT_FAILURE);
    }

    const std::string new_process_name = common::MemSPrintf(STREAMER_NAME "_%s", sid);
    const char* new_name = new_process_name.c_str();
#if defined(OS_LINUX)
    for (int i = 0; i < process_argc_; ++i) {
      memset(process_argv_[i], 0, strlen(process_argv_[i]));
    }
    char* app_name = process_argv_[0];
    strncpy(app_name, new_name, new_process_name.length());
    app_name[new_process_name.length()] = 0;
    prctl(PR_SET_NAME, new_name);
#elif defined(OS_FREEBSD)
    setproctitle(new_name);
#else
#pragma message "Please implement"
#endif

#if PIPE
#if !defined(TEST)
    // close not needed pipes
    common::ErrnoError errn = common::file_system::close_descriptor(read_responce_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    errn = common::file_system::close_descriptor(write_requests_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
#endif
    pipe::Client* client = new pipe::Client(nullptr, read_command_client, write_responce_client);
#else
    // close not needed sock
    common::ErrnoError errn = common::file_system::close_descriptor(parent_sock);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    tcp::Client* client = new tcp::Client(nullptr, common::net::socket_info(child_sock));
#endif

    client->SetName(sid);
    int res = stream_exec_func(new_name, config_args.get(), client);
    ignore_result(client->Close());
    delete client;
    dlclose(handle);
    _exit(res);
  } else if (pid < 0) {
    ERROR_LOG() << "Failed to start children!";
  } else {
#if PIPE
    // close not needed pipes
    common::ErrnoError errn = common::file_system::close_descriptor(read_command_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    errn = common::file_system::close_descriptor(write_responce_client);
    if (err) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    pipe::Client* client = new pipe::Client(loop_, read_responce_client, write_requests_client);
#else
    // close not needed sock
    common::ErrnoError errn = common::file_system::close_descriptor(child_sock);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    tcp::Client* client = new tcp::Client(loop_, common::net::socket_info(parent_sock));
#endif
    client->SetName(sid);
    bool registered = loop_->RegisterClient(client);
    if (!registered) {
      return common::make_errno_error("Can't register communication pipe", EAGAIN);
    }
    ChildStream* new_channel = new ChildStream(loop_, sha);
    new_channel->SetClient(client);
    loop_->RegisterChild(new_channel, pid);
  }

  return common::ErrnoError();
}

}  // namespace server
}  // namespace fastocloud
