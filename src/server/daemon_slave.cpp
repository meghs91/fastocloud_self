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

#include <unistd.h>
#if defined(OS_WIN)
#include <winsock2.h>
#else
#include <signal.h>
#endif

#include <iostream>

#include <common/file_system/file.h>
#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>
#if defined(OS_POSIX)
#include <common/utils.h>
#endif

#include "server/process_slave_wrapper.h"

#define HELP_TEXT                       \
  "Usage: " STREAMER_SERVICE_NAME       \
  " [options]\n"                        \
  "  Manipulate " STREAMER_SERVICE_NAME \
  ".\n\n"                               \
  "    --version  display version\n"    \
  "    --daemon   run as a daemon\n"    \
  "    --stop     stop running instance\n"

namespace {

#if defined(OS_WIN)
struct WinsockInit {
  WinsockInit() {
    WSADATA d;
    if (WSAStartup(0x202, &d) != 0) {
      _exit(1);
    }
  }
  ~WinsockInit() { WSACleanup(); }
} winsock_init;
#else
struct SigIgnInit {
  SigIgnInit() { signal(SIGPIPE, SIG_IGN); }
} sig_init;
#endif

const size_t kMaxSizeLogFile = 10 * 1024 * 1024;  // 10 MB
}  // namespace

int main(int argc, char** argv, char** envp) {
  bool run_as_daemon = false;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--version") == 0) {
      std::cout << PROJECT_VERSION_HUMAN << std::endl;
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "--daemon") == 0) {
      run_as_daemon = true;
    } else if (strcmp(argv[i], "--stop") == 0) {
      fastocloud::server::Config config;
      common::ErrnoError err = fastocloud::server::load_config_from_file(CONFIG_PATH, &config);
      if (err) {
        std::cerr << "Can't read config, file path: " << CONFIG_PATH << ", error: " << err->GetDescription()
                  << std::endl;
        return EXIT_FAILURE;
      }

      err = fastocloud::server::ProcessSlaveWrapper::SendStopDaemonRequest(config);
      if (err) {
        std::cerr << "Stop command failed error: " << err->GetDescription() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      std::cout << HELP_TEXT << std::endl;
      return EXIT_SUCCESS;
    } else {
      std::cerr << HELP_TEXT << std::endl;
      return EXIT_FAILURE;
    }
  }

  fastocloud::server::Config config;
  common::ErrnoError err = fastocloud::server::load_config_from_file(CONFIG_PATH, &config);
  if (err) {
    std::cerr << "Can't read config, file path: " << CONFIG_PATH << ", error: " << err->GetDescription() << std::endl;
    return EXIT_FAILURE;
  }

  if (run_as_daemon) {
#if defined(OS_POSIX)
    if (!common::create_as_daemon()) {
      return EXIT_FAILURE;
    }
#endif
  }

  common::logging::INIT_LOGGER(STREAMER_SERVICE_NAME, config.log_path, config.log_level,
                               kMaxSizeLogFile);  // initialization of logging system

  const pid_t daemon_pid = getpid();
  const std::string folder_path_to_pid = common::file_system::get_dir_path(PIDFILE_PATH);
  if (folder_path_to_pid.empty()) {
    ERROR_LOG() << "Can't get pid file path: " << PIDFILE_PATH;
    return EXIT_FAILURE;
  }

  if (!common::file_system::is_directory_exist(folder_path_to_pid)) {
    if (!common::file_system::create_directory(folder_path_to_pid, true)) {
      ERROR_LOG() << "Pid file directory not exists, pid file path: " << PIDFILE_PATH;
      return EXIT_FAILURE;
    }
  }

  err = common::file_system::node_access(folder_path_to_pid);
  if (err) {
    ERROR_LOG() << "Can't have permissions to create, pid file path: " << PIDFILE_PATH;
    return EXIT_FAILURE;
  }

  common::file_system::File pidfile;
  err = pidfile.Open(PIDFILE_PATH, common::file_system::File::FLAG_CREATE | common::file_system::File::FLAG_WRITE);
  if (err) {
    ERROR_LOG() << "Can't open pid file path: " << PIDFILE_PATH;
    return EXIT_FAILURE;
  }

  err = pidfile.Lock();
  if (err) {
    ERROR_LOG() << "Can't lock pid file path: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  std::string pid_str = common::MemSPrintf("%ld\n", static_cast<long>(daemon_pid));
  size_t writed;
  err = pidfile.WriteBuffer(pid_str, &writed);
  if (err) {
    ERROR_LOG() << "Failed to write pid file path: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  // start
  fastocloud::server::ProcessSlaveWrapper wrapper(config);
  NOTICE_LOG() << "Running " PROJECT_VERSION_HUMAN << " in " << (run_as_daemon ? "daemon" : "common") << " mode";

  for (char** env = envp; *env != nullptr; env++) {
    char* cur_env = *env;
    DEBUG_LOG() << cur_env;
  }

  int res = wrapper.Exec(argc, argv);
  NOTICE_LOG() << "Quiting " PROJECT_VERSION_HUMAN;

  err = pidfile.Unlock();
  if (err) {
    ERROR_LOG() << "Failed to unlock pidfile: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  err = pidfile.Close();
  if (err) {
    ERROR_LOG() << "Failed to close pidfile: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  err = common::file_system::remove_file(PIDFILE_PATH);
  if (err) {
    WARNING_LOG() << "Can't remove file: " << PIDFILE_PATH << ", error: " << err->GetDescription();
  }
  return res;
}
