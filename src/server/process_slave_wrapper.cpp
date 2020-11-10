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

#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <common/convert2string.h>
#include <common/daemon/commands/activate_info.h>
#include <common/daemon/commands/get_log_info.h>
#include <common/daemon/commands/stop_info.h>
#include <common/file_system/string_path_utils.h>
#include <common/license/expire_license.h>
#include <common/net/http_client.h>
#include <common/net/net.h>

#include "base/config_fields.h"
#include "base/constants.h"
#include "base/inputs_outputs.h"
#include "base/utils.h"

#include "gpu_stats/perf_monitor.h"

#include "server/child_stream.h"
#include "server/daemon/client.h"
#include "server/daemon/commands.h"
#include "server/daemon/commands_info/service/details/shots.h"
#include "server/daemon/commands_info/service/prepare_info.h"
#include "server/daemon/commands_info/service/server_info.h"
#include "server/daemon/commands_info/service/sync_info.h"
#include "server/daemon/commands_info/stream/get_log_info.h"
#include "server/daemon/commands_info/stream/restart_info.h"
#include "server/daemon/commands_info/stream/start_info.h"
#include "server/daemon/commands_info/stream/stop_info.h"
#include "server/daemon/server.h"
#include "server/http/handler.h"
#include "server/http/server.h"
#include "server/options/options.h"
#include "server/vods/handler.h"
#include "server/vods/server.h"

#include "stream_commands/commands.h"

#include "utils/m3u8_reader.h"

#if defined(OS_WIN)
#undef SetPort
#endif

namespace {

common::Optional<common::file_system::ascii_file_string_path> MakeStreamLogPath(const std::string& feedback_dir) {
  common::file_system::ascii_directory_string_path dir(feedback_dir);
  return dir.MakeFileStringPath(LOGS_FILE_NAME);
}

common::Optional<common::file_system::ascii_file_string_path> MakeStreamPipelinePath(const std::string& feedback_dir) {
  common::file_system::ascii_directory_string_path dir(feedback_dir);
  return dir.MakeFileStringPath(DUMP_FILE_NAME);
}

}  // namespace

namespace fastocloud {
namespace server {
namespace {
typedef VodsHandler CodsHandler;
typedef VodsServer CodsServer;

bool CheckIsFullVod(const common::file_system::ascii_file_string_path& file) {
  utils::M3u8Reader reader;
  if (!reader.Parse(file)) {
    return false;
  }

  common::file_system::ascii_directory_string_path dir(file.GetDirectory());
  const auto chunks = reader.GetChunks();
  for (const utils::ChunkInfo& chunk : chunks) {
    const auto chunk_path = dir.MakeFileStringPath(chunk.path);
    if (!chunk_path) {
      return false;
    }

    if (!common::file_system::is_file_exist(chunk_path->GetPath())) {
      return false;
    }
  }

  return true;
}
}  // namespace

struct ProcessSlaveWrapper::NodeStats {
  NodeStats() : prev(), prev_nshot(), gpu_load(0), timestamp(common::time::current_utc_mstime()) {}

  service::CpuShot prev;
  service::NetShot prev_nshot;
  int gpu_load;
  fastotv::timestamp_t timestamp;
};

ProcessSlaveWrapper::ProcessSlaveWrapper(const Config& config)
    : config_(config),
      process_argc_(0),
      process_argv_(nullptr),
      loop_(nullptr),
      http_server_(nullptr),
      http_handler_(nullptr),
      vods_server_(nullptr),
      vods_handler_(nullptr),
      cods_server_(nullptr),
      cods_handler_(nullptr),
      ping_client_timer_(INVALID_TIMER_ID),
      check_cods_vods_timer_(INVALID_TIMER_ID),
      check_old_files_timer_(INVALID_TIMER_ID),
      node_stats_timer_(INVALID_TIMER_ID),
      quit_cleanup_timer_(INVALID_TIMER_ID),
      check_license_timer_(INVALID_TIMER_ID),
      node_stats_(new NodeStats),
      vods_links_(),
      cods_links_(),
      folders_for_monitor_() {
  loop_ = new DaemonServer(config.host, this);
  loop_->SetName("client_server");

  http_handler_ = new HttpHandler(this);
  http_server_ = new HttpServer(config.http_host, http_handler_);
  http_server_->SetName("http_server");

  vods_handler_ = new VodsHandler(this);
  vods_server_ = new VodsServer(config.vods_host, vods_handler_);
  vods_server_->SetName("vods_server");

  cods_handler_ = new CodsHandler(this);
  cods_server_ = new CodsServer(config.cods_host, cods_handler_);
  cods_server_->SetName("cods_server");
}

common::ErrnoError ProcessSlaveWrapper::SendStopDaemonRequest(const Config& config) {
  if (!config.IsValid()) {
    return common::make_errno_error_inval();
  }

  common::net::HostAndPort host = config.host;
  if (host.GetHost() == PROJECT_NAME_LOWERCASE) {  // docker image
    host = common::net::HostAndPort::CreateLocalHostIPV4(host.GetPort());
  }

  common::net::socket_info client_info;
  common::ErrnoError err = common::net::connect(host, common::net::ST_SOCK_STREAM, nullptr, &client_info);
  if (err) {
    return err;
  }

  std::unique_ptr<ProtocoledDaemonClient> connection(new ProtocoledDaemonClient(nullptr, client_info));
  err = connection->StopMe();
  if (err) {
    ignore_result(connection->Close());
    return err;
  }

  ignore_result(connection->Close());
  return common::ErrnoError();
}

ProcessSlaveWrapper::~ProcessSlaveWrapper() {
  destroy(&cods_server_);
  destroy(&cods_handler_);
  destroy(&vods_server_);
  destroy(&vods_handler_);
  destroy(&http_server_);
  destroy(&http_handler_);
  destroy(&loop_);
  destroy(&node_stats_);
}

int ProcessSlaveWrapper::Exec(int argc, char** argv) {
  process_argc_ = argc;
  process_argv_ = argv;

  // gpu statistic monitor
  std::thread perf_thread;
  gpu_stats::IPerfMonitor* perf_monitor = gpu_stats::CreatePerfMonitor(&node_stats_->gpu_load);
  if (perf_monitor) {
    perf_thread = std::thread([perf_monitor] { perf_monitor->Exec(); });
  }

  HttpServer* http_server = static_cast<HttpServer*>(http_server_);
  std::thread http_thread = std::thread([http_server] {
    common::ErrnoError err = http_server->Bind(true);
    if (err) {
      ERROR_LOG() << "HttpServer error: " << err->GetDescription();
      return;
    }

    err = http_server->Listen(5);
    if (err) {
      ERROR_LOG() << "HttpServer error: " << err->GetDescription();
      return;
    }

    int res = http_server->Exec();
    UNUSED(res);
  });

  VodsServer* vods_server = static_cast<VodsServer*>(vods_server_);
  std::thread vods_thread = std::thread([vods_server] {
    common::ErrnoError err = vods_server->Bind(true);
    if (err) {
      ERROR_LOG() << "VodsServer error: " << err->GetDescription();
      return;
    }

    err = vods_server->Listen(5);
    if (err) {
      ERROR_LOG() << "VodsServer error: " << err->GetDescription();
      return;
    }

    int res = vods_server->Exec();
    UNUSED(res);
  });

  CodsServer* cods_server = static_cast<CodsServer*>(cods_server_);
  std::thread cods_thread = std::thread([cods_server] {
    common::ErrnoError err = cods_server->Bind(true);
    if (err) {
      ERROR_LOG() << "CodsServer error: " << err->GetDescription();
      return;
    }

    err = cods_server->Listen(5);
    if (err) {
      ERROR_LOG() << "CodsServer error: " << err->GetDescription();
      return;
    }

    int res = cods_server->Exec();
    UNUSED(res);
  });

  int res = EXIT_FAILURE;
  DaemonServer* server = static_cast<DaemonServer*>(loop_);
  common::ErrnoError err = server->Bind(true);
  if (err) {
    ERROR_LOG() << "DaemonServer error: " << err->GetDescription();
    goto finished;
  }

  err = server->Listen(5);
  if (err) {
    ERROR_LOG() << "DaemonServer error: " << err->GetDescription();
    goto finished;
  }

  node_stats_->prev = service::GetMachineCpuShot();
  node_stats_->prev_nshot = service::GetMachineNetShot();
  node_stats_->timestamp = common::time::current_utc_mstime();

  res = server->Exec();

finished:
  vods_thread.join();
  cods_thread.join();
  http_thread.join();
  if (perf_monitor) {
    perf_monitor->Stop();
  }
  if (perf_thread.joinable()) {
    perf_thread.join();
  }
  delete perf_monitor;
  return res;
}

void ProcessSlaveWrapper::PreLooped(common::libev::IoLoop* server) {
  ping_client_timer_ = server->CreateTimer(ping_timeout_clients_seconds, true);
  check_cods_vods_timer_ = server->CreateTimer(config_.cods_ttl / 2, true);
  check_old_files_timer_ = server->CreateTimer(config_.files_ttl / 10, true);
  node_stats_timer_ = server->CreateTimer(node_stats_send_seconds, true);
  check_license_timer_ = server->CreateTimer(check_license_timeout_seconds, true);
}

void ProcessSlaveWrapper::Accepted(common::libev::IoClient* client) {
  UNUSED(client);
}

void ProcessSlaveWrapper::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
}

void ProcessSlaveWrapper::Closed(common::libev::IoClient* client) {
  UNUSED(client);
}

void ProcessSlaveWrapper::TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) {
  if (ping_client_timer_ == id) {
    std::vector<common::libev::IoClient*> online_clients = server->GetClients();
    for (size_t i = 0; i < online_clients.size(); ++i) {
      common::libev::IoClient* client = online_clients[i];
      ProtocoledDaemonClient* dclient = dynamic_cast<ProtocoledDaemonClient*>(client);
      if (dclient && dclient->IsVerified()) {
        common::ErrnoError err = dclient->Ping();
        if (err) {
          DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
          ignore_result(dclient->Close());
          delete dclient;
        } else {
          INFO_LOG() << "Sent ping to client[" << client->GetFormatedName() << "], from server["
                     << server->GetFormatedName() << "], " << online_clients.size() << " client(s) connected.";
        }
      }
    }
  } else if (check_cods_vods_timer_ == id) {
    fastotv::timestamp_t current_time = common::time::current_utc_mstime();
    const auto copy_cods = cods_links_.Copy();
    for (auto it = copy_cods.begin(); it != copy_cods.end(); ++it) {
      serialized_stream_t conf = it->second;
      fastotv::stream_id_t sid = GetSid(conf);
      Child* cod = FindChildByID(sid);
      if (cod) {
        fastotv::timestamp_t cod_last_update = cod->GetLastUpdate();
        fastotv::timestamp_t ts_diff = current_time - cod_last_update;
        if (ts_diff > config_.cods_ttl * 1000) {
          ignore_result(cod->Stop());
        }
      }
    }
  } else if (check_old_files_timer_ == id) {
    for (auto it = folders_for_monitor_.begin(); it != folders_for_monitor_.end(); ++it) {
      const common::file_system::ascii_directory_string_path folder = *it;
      const time_t max_life_time = common::time::current_utc_mstime() / 1000 - config_.files_ttl;
      RemoveOldFilesByTime(folder, max_life_time, "*" CHUNK_EXT, true);
    }
  } else if (node_stats_timer_ == id) {
    const std::string node_stats = MakeServiceStats(0);
    fastotv::protocol::request_t req;
    common::Error err_ser = StatisitcServiceBroadcast(node_stats, &req);
    if (err_ser) {
      return;
    }

    BroadcastClients(req);
  } else if (quit_cleanup_timer_ == id) {
    vods_server_->Stop();
    cods_server_->Stop();
    http_server_->Stop();
    loop_->Stop();
  } else if (check_license_timer_ == id) {
    CheckLicenseExpired();
  }
}

void ProcessSlaveWrapper::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void ProcessSlaveWrapper::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void ProcessSlaveWrapper::ChildStatusChanged(common::libev::IoChild* child, int status, int signal) {
  ChildStream* channel = static_cast<ChildStream*>(child);
  channel->CleanUp();
  const auto sid = channel->GetStreamID();

  INFO_LOG() << "Successful finished children id: " << sid << "\nStream id: " << sid
             << ", exit with status: " << (status ? "FAILURE" : "SUCCESS") << ", signal: " << signal;

  loop_->UnRegisterChild(child);

  delete channel;

  stream::QuitStatusInfo ch_status_info(sid, status, signal);
  fastotv::protocol::request_t req;
  common::Error err_ser = QuitStatusStreamBroadcast(ch_status_info, &req);
  if (err_ser) {
    return;
  }

  BroadcastClients(req);
}

void ProcessSlaveWrapper::StopImpl() {
  DaemonServer* server = static_cast<DaemonServer*>(loop_);
  auto childs = server->GetChilds();
  for (auto* child : childs) {
    ChildStream* channel = static_cast<ChildStream*>(child);
    ignore_result(channel->Stop());
  }

  quit_cleanup_timer_ = loop_->CreateTimer(cleanup_seconds, false);
}

Child* ProcessSlaveWrapper::FindChildByID(fastotv::stream_id_t cid) const {
  CHECK(loop_->IsLoopThread());
  DaemonServer* server = static_cast<DaemonServer*>(loop_);
  auto childs = server->GetChilds();
  for (auto* child : childs) {
    Child* channel = static_cast<Child*>(child);
    if (channel->GetStreamID() == cid) {
      return channel;
    }
  }

  return nullptr;
}

void ProcessSlaveWrapper::BroadcastClients(const fastotv::protocol::request_t& req) {
  std::vector<common::libev::IoClient*> clients = loop_->GetClients();
  for (size_t i = 0; i < clients.size(); ++i) {
    ProtocoledDaemonClient* dclient = dynamic_cast<ProtocoledDaemonClient*>(clients[i]);
    if (dclient && dclient->IsVerified()) {
      common::ErrnoError err = dclient->WriteRequest(req);
      if (err) {
        WARNING_LOG() << "BroadcastClients error: " << err->GetDescription();
      }
    }
  }
}

common::ErrnoError ProcessSlaveWrapper::DaemonDataReceived(ProtocoledDaemonClient* dclient) {
  CHECK(loop_->IsLoopThread());
  std::string input_command;
  common::ErrnoError err = dclient->ReadCommand(&input_command);
  if (err) {
    return err;  // i don't want handle spam, comand must be foramated according
                 // protocol
  }

  fastotv::protocol::request_t* req = nullptr;
  fastotv::protocol::response_t* resp = nullptr;
  common::Error err_parse = common::protocols::json_rpc::ParseJsonRPC(input_command, &req, &resp);
  if (err_parse) {
    const std::string err_str = err_parse->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  if (req) {
    DEBUG_LOG() << "Received daemon request: " << input_command;
    err = HandleRequestServiceCommand(dclient, req);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete req;
  } else if (resp) {
    DEBUG_LOG() << "Received daemon responce: " << input_command;
    err = HandleResponceServiceCommand(dclient, resp);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete resp;
  } else {
    DNOTREACHED();
    return common::make_errno_error("Invalid command type.", EINVAL);
  }

  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::StreamDataReceived(stream_client_t* pipe_client) {
  CHECK(loop_->IsLoopThread());
  std::string input_command;
  common::ErrnoError err = pipe_client->ReadCommand(&input_command);
  if (err) {
    return err;  // i don't want to handle spam, command must be formated according protocol
  }

  fastotv::protocol::request_t* req = nullptr;
  fastotv::protocol::response_t* resp = nullptr;
  common::Error err_parse = common::protocols::json_rpc::ParseJsonRPC(input_command, &req, &resp);
  if (err_parse) {
    const std::string err_str = err_parse->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  if (req) {
    INFO_LOG() << "Received stream responce: " << input_command;
    err = HandleRequestStreamsCommand(pipe_client, req);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete req;
  } else if (resp) {
    INFO_LOG() << "Received stream responce: " << input_command;
    err = HandleResponceStreamsCommand(pipe_client, resp);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete resp;
  } else {
    NOTREACHED();
    return common::make_errno_error("Invalid command type.", EINVAL);
  }
  return common::ErrnoError();
}

void ProcessSlaveWrapper::DataReceived(common::libev::IoClient* client) {
  if (ProtocoledDaemonClient* dclient = dynamic_cast<ProtocoledDaemonClient*>(client)) {
    common::ErrnoError err = DaemonDataReceived(dclient);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      ignore_result(dclient->Close());
      delete dclient;
    }
  } else if (stream_client_t* pipe_client = dynamic_cast<stream_client_t*>(client)) {
    common::ErrnoError err = StreamDataReceived(pipe_client);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      DaemonServer* server = static_cast<DaemonServer*>(loop_);
      auto childs = server->GetChilds();
      for (auto* child : childs) {
        ChildStream* channel = static_cast<ChildStream*>(child);
        if (pipe_client == channel->GetClient()) {
          channel->SetClient(nullptr);
          break;
        }
      }

      ignore_result(pipe_client->Close());
      delete pipe_client;
    }
  } else {
    NOTREACHED();
  }
}

void ProcessSlaveWrapper::DataReadyToWrite(common::libev::IoClient* client) {
  UNUSED(client);
}

void ProcessSlaveWrapper::PostLooped(common::libev::IoLoop* server) {
  if (check_cods_vods_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(check_cods_vods_timer_);
    check_cods_vods_timer_ = INVALID_TIMER_ID;
  }

  if (check_old_files_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(check_old_files_timer_);
    check_old_files_timer_ = INVALID_TIMER_ID;
  }

  if (ping_client_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(ping_client_timer_);
    ping_client_timer_ = INVALID_TIMER_ID;
  }

  if (node_stats_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(node_stats_timer_);
    node_stats_timer_ = INVALID_TIMER_ID;
  }

  if (quit_cleanup_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(quit_cleanup_timer_);
    quit_cleanup_timer_ = INVALID_TIMER_ID;
  }

  if (check_license_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(check_license_timer_);
    check_license_timer_ = INVALID_TIMER_ID;
  }
}

void ProcessSlaveWrapper::OnHttpRequest(common::libev::http::HttpClient* client,
                                        const file_path_t& file,
                                        common::http::http_status* recommend_status) {
  if (client->GetServer() == vods_server_) {
    std::string ext = file.GetExtension();
    bool is_m3u8 = common::EqualsASCII(ext, M3U8_EXTENSION, false);
    if (is_m3u8) {
      const common::file_system::ascii_directory_string_path http_root(file.GetDirectory());
      auto config = vods_links_.Find(http_root);
      if (!config) {
        if (recommend_status) {
          *recommend_status = common::http::HS_FORBIDDEN;
        }
        return;
      }

      bool is_full_vod = CheckIsFullVod(file);
      if (is_full_vod) {
        if (recommend_status) {
          *recommend_status = common::http::HS_OK;
        }
        return;
      }

      loop_->ExecInLoopThread([this, config]() {
        common::ErrnoError errn = CreateChildStream(config);
        if (errn) {
          DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
        }
      });
      if (recommend_status) {
        *recommend_status = common::http::HS_ACCEPTED;
      }
      return;
    }
  } else if (client->GetServer() == cods_server_) {
    std::string ext = file.GetExtension();
    bool is_m3u8 = common::EqualsASCII(ext, M3U8_EXTENSION, false);
    bool is_ts = common::EqualsASCII(ext, TS_EXTENSION, false);
    if (is_m3u8 || is_ts) {
      const common::file_system::ascii_directory_string_path http_root(file.GetDirectory());
      auto config = cods_links_.Find(http_root);
      if (!config) {
        if (recommend_status) {
          *recommend_status = common::http::HS_FORBIDDEN;
        }
        return;
      }

      loop_->ExecInLoopThread([this, is_m3u8, config]() {
        if (is_m3u8) {
          common::ErrnoError errn = CreateChildStream(config);
          if (errn) {
            DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
          }
        }

        fastotv::stream_id_t sid = GetSid(config);
        Child* cod = FindChildByID(sid);
        if (cod) {
          cod->UpdateTimestamp();
        }
      });
      if (is_m3u8) {
        if (recommend_status) {
          *recommend_status = common::http::HS_ACCEPTED;
        }
      }
      return;
    }
  }

  if (recommend_status) {
    *recommend_status = common::http::HS_OK;
  }
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStopService(ProtocoledDaemonClient* dclient,
                                                                       const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    const auto info = dclient->GetInfo();
    common::net::HostAndPort host(info.host(), info.port());
    if (!host.IsLocalHost()) {
      return common::make_errno_error_inval();
    }
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jstop = json_tokener_parse(params_ptr);
    if (!jstop) {
      return common::make_errno_error_inval();
    }

    common::daemon::commands::StopInfo stop_info;
    common::Error err_des = stop_info.DeSerialize(jstop);
    json_object_put(jstop);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    if (quit_cleanup_timer_ != INVALID_TIMER_ID) {
      // in progress
      return dclient->StopFail(req->id, common::make_error("Stop service in progress..."));
    }

    StopImpl();
    return dclient->StopSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleResponcePingService(ProtocoledDaemonClient* dclient,
                                                                  const fastotv::protocol::response_t* resp) {
  UNUSED(dclient);
  CHECK(loop_->IsLoopThread());
  if (resp->IsMessage()) {
    const char* params_ptr = resp->message->result.c_str();
    json_object* jclient_ping = json_tokener_parse(params_ptr);
    if (!jclient_ping) {
      return common::make_errno_error_inval();
    }

    common::daemon::commands::ClientPingInfo client_ping_info;
    common::Error err_des = client_ping_info.DeSerialize(jclient_ping);
    json_object_put(jclient_ping);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }
    return common::ErrnoError();
  }
  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::CreateChildStream(const serialized_stream_t& config_args) {
  CHECK(loop_->IsLoopThread());
  common::ErrnoError err = options::ValidateConfig(config_args);
  if (err) {
    return err;
  }

  StreamInfo sha;
  std::string feedback_dir;
  std::string data_dir;
  common::logging::LOG_LEVEL logs_level;
  err = MakeStreamInfo(config_args, true, &sha, &feedback_dir, &data_dir, &logs_level);
  if (err) {
    return err;
  }

  Child* stream = FindChildByID(sha.id);
  if (stream) {
    return common::make_errno_error(common::MemSPrintf("Stream with id: %s exist, skip request.", sha.id), EEXIST);
  }

  config_args->Insert(STREAM_LINK_PATH_FIELD, common::Value::CreateStringValueFromBasicString(config_.streamlink_path));
  return CreateChildStreamImpl(config_args, sha);
}

common::ErrnoError ProcessSlaveWrapper::StopChildStream(const serialized_stream_t& config_args) {
  fastotv::stream_id_t sid = GetSid(config_args);
  return StopChildStreamImpl(sid);
}

common::ErrnoError ProcessSlaveWrapper::StopChildStreamImpl(fastotv::stream_id_t sid) {
  CHECK(loop_->IsLoopThread());
  Child* stream = FindChildByID(sid);
  if (!stream) {
    return common::make_errno_error(common::MemSPrintf("Stream with id: %s not exist, skip request.", sid), EINVAL);
  }

  return stream->Stop();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestChangedSourcesStream(stream_client_t* pclient,
                                                                          const fastotv::protocol::request_t* req) {
  UNUSED(pclient);
  CHECK(loop_->IsLoopThread());
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jrequest_changed_sources = json_tokener_parse(params_ptr);
    if (!jrequest_changed_sources) {
      return common::make_errno_error_inval();
    }

    ChangedSouresInfo ch_sources_info;
    common::Error err_des = ch_sources_info.DeSerialize(jrequest_changed_sources);
    json_object_put(jrequest_changed_sources);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    fastotv::protocol::request_t req;
    common::Error err_ser = ChangedSourcesStreamBroadcast(ch_sources_info, &req);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    BroadcastClients(req);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestStatisticStream(stream_client_t* pclient,
                                                                     const fastotv::protocol::request_t* req) {
  UNUSED(pclient);
  CHECK(loop_->IsLoopThread());
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jrequest_stat = json_tokener_parse(params_ptr);
    if (!jrequest_stat) {
      return common::make_errno_error_inval();
    }

    StatisticInfo stat;
    common::Error err_des = stat.DeSerialize(jrequest_stat);
    json_object_put(jrequest_stat);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    fastotv::protocol::request_t req;
    common::Error err_ser = StatisitcStreamBroadcast(stat, &req);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    BroadcastClients(req);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

#if defined(MACHINE_LEARNING)
common::ErrnoError ProcessSlaveWrapper::HandleRequestMlNotificationStream(stream_client_t* pclient,
                                                                          const fastotv::protocol::request_t* req) {
  UNUSED(pclient);
  CHECK(loop_->IsLoopThread());
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jrequest_notif = json_tokener_parse(params_ptr);
    if (!jrequest_notif) {
      return common::make_errno_error_inval();
    }

    fastotv::commands_info::ml::NotificationInfo notif;
    common::Error err_des = notif.DeSerialize(jrequest_notif);
    json_object_put(jrequest_notif);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    fastotv::protocol::request_t req;
    common::Error err_ser = MlNotificationStreamBroadcast(notif, &req);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    BroadcastClients(req);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}
#endif

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStartStream(ProtocoledDaemonClient* dclient,
                                                                       const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->HaveFullAccess()) {
    return common::make_errno_error("Don't have permissions", EINTR);
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jstart_info = json_tokener_parse(params_ptr);
    if (!jstart_info) {
      return common::make_errno_error_inval();
    }

    stream::StartInfo start_info;
    common::Error err_des = start_info.DeSerialize(jstart_info);
    json_object_put(jstart_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    common::ErrnoError errn = CreateChildStream(start_info.GetConfig());
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
      ignore_result(dclient->StartStreamFail(req->id, common::make_error_from_errno(errn)));
      return errn;
    }

    return dclient->StartStreamSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStopStream(ProtocoledDaemonClient* dclient,
                                                                      const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->HaveFullAccess()) {
    return common::make_errno_error("Don't have permissions", EINTR);
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jstop_info = json_tokener_parse(params_ptr);
    if (!jstop_info) {
      return common::make_errno_error_inval();
    }

    stream::StopInfo stop_info;
    common::Error err_des = stop_info.DeSerialize(jstop_info);
    json_object_put(jstop_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    common::ErrnoError errn = StopChildStreamImpl(stop_info.GetStreamID());
    if (errn) {
      return dclient->StopFail(req->id, common::make_error_from_errno(errn));
    }

    return dclient->StopStreamSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientRestartStream(ProtocoledDaemonClient* dclient,
                                                                         const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->HaveFullAccess()) {
    return common::make_errno_error("Don't have permissions", EINTR);
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jrestart_info = json_tokener_parse(params_ptr);
    if (!jrestart_info) {
      return common::make_errno_error_inval();
    }

    stream::RestartInfo restart_info;
    common::Error err_des = restart_info.DeSerialize(jrestart_info);
    json_object_put(jrestart_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    Child* chan = FindChildByID(restart_info.GetStreamID());
    if (!chan) {
      return dclient->ReStartStreamFail(req->id, common::make_error("Stream not found"));
    }

    ignore_result(chan->Restart());
    return dclient->ReStartStreamSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientGetLogStream(ProtocoledDaemonClient* dclient,
                                                                        const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->HaveFullAccess()) {
    return common::make_errno_error("Don't have permissions", EINTR);
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jlog = json_tokener_parse(params_ptr);
    if (!jlog) {
      return common::make_errno_error_inval();
    }

    stream::GetLogInfo log_info;
    common::Error err_des = log_info.DeSerialize(jlog);
    json_object_put(jlog);
    if (err_des) {
      ignore_result(dclient->GetLogStreamFail(req->id, err_des));
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    const auto remote_log_path = log_info.GetLogPath();
    if (!remote_log_path.SchemeIsHTTPOrHTTPS()) {
      common::ErrnoError errn = common::make_errno_error("Not supported protocol", EAGAIN);
      ignore_result(dclient->GetLogStreamFail(req->id, common::make_error_from_errno(errn)));
      return errn;
    }

    const auto stream_log_file = MakeStreamLogPath(log_info.GetFeedbackDir());
    if (!stream_log_file) {
      common::ErrnoError errn = common::make_errno_error("Can't generate log stream path", EAGAIN);
      ignore_result(dclient->GetLogStreamFail(req->id, common::make_error_from_errno(errn)));
      return errn;
    }

    common::Error err = common::net::PostHttpFile(*stream_log_file, remote_log_path);
    if (err) {
      ignore_result(dclient->GetLogStreamFail(req->id, err));
      const std::string err_str = err->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }
    return dclient->GetLogStreamSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientGetPipelineStream(ProtocoledDaemonClient* dclient,
                                                                             const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->HaveFullAccess()) {
    return common::make_errno_error("Don't have permissions", EINTR);
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jpipe = json_tokener_parse(params_ptr);
    if (!jpipe) {
      return common::make_errno_error_inval();
    }

    stream::GetLogInfo pipeline_info;
    common::Error err_des = pipeline_info.DeSerialize(jpipe);
    json_object_put(jpipe);
    if (err_des) {
      ignore_result(dclient->GetPipeStreamFail(req->id, err_des));
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    const auto remote_log_path = pipeline_info.GetLogPath();
    if (!remote_log_path.SchemeIsHTTPOrHTTPS()) {
      common::ErrnoError errn = common::make_errno_error("Not supported protocol", EAGAIN);
      ignore_result(dclient->GetPipeStreamFail(req->id, common::make_error_from_errno(errn)));
      return errn;
    }

    const auto pipe_file = MakeStreamPipelinePath(pipeline_info.GetFeedbackDir());
    if (!pipe_file) {
      common::ErrnoError errn = common::make_errno_error("Can't generate log stream path", EAGAIN);
      ignore_result(dclient->GetLogStreamFail(req->id, common::make_error_from_errno(errn)));
      return errn;
    }

    common::Error err = common::net::PostHttpFile(*pipe_file, remote_log_path);
    if (err) {
      ignore_result(dclient->GetPipeStreamFail(req->id, err));
      const std::string err_str = err->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }
    return dclient->GetPipeStreamSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientPrepareService(ProtocoledDaemonClient* dclient,
                                                                          const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jservice_state = json_tokener_parse(params_ptr);
    if (!jservice_state) {
      return common::make_errno_error_inval();
    }

    service::PrepareInfo state_info;
    common::Error err_des = state_info.DeSerialize(jservice_state);
    json_object_put(jservice_state);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    folders_for_monitor_.clear();

    const auto http_root = HttpHandler::http_directory_path_t(state_info.GetHlsDirectory());
    static_cast<HttpHandler*>(http_handler_)->SetHttpRoot(http_root);
    folders_for_monitor_.push_back(http_root);

    const auto vods_root = VodsHandler::http_directory_path_t(state_info.GetVodsDirectory());
    static_cast<VodsHandler*>(vods_handler_)->SetHttpRoot(vods_root);
    folders_for_monitor_.push_back(vods_root);

    const auto cods_root = CodsHandler::http_directory_path_t(state_info.GetCodsDirectory());
    static_cast<CodsHandler*>(cods_handler_)->SetHttpRoot(cods_root);
    folders_for_monitor_.push_back(cods_root);

    const auto timeshift_root = CodsHandler::http_directory_path_t(state_info.GetTimeshiftsDirectory());
    folders_for_monitor_.push_back(timeshift_root);

    service::Directories dirs(state_info);
    std::string resp_str = service::MakeDirectoryResponce(dirs);
    return dclient->PrepareServiceSuccess(req->id, resp_str);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientSyncService(ProtocoledDaemonClient* dclient,
                                                                       const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jservice_state = json_tokener_parse(params_ptr);
    if (!jservice_state) {
      return common::make_errno_error_inval();
    }

    service::SyncInfo sync_info;
    common::Error err_des = sync_info.DeSerialize(jservice_state);
    json_object_put(jservice_state);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    // refresh vods
    // vods_links_.Clear();
    // cods_links_.Clear();
    for (StreamConfig config : sync_info.GetStreams()) {
      AddStreamLine(config);
    }

    return dclient->SyncServiceSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

void ProcessSlaveWrapper::AddStreamLine(const serialized_stream_t& config_args) {
  CHECK(loop_->IsLoopThread());
  common::ErrnoError err = options::ValidateConfig(config_args);
  if (err) {
    return;
  }

  StreamInfo sha;
  std::string feedback_dir;
  std::string data_dir;
  common::logging::LOG_LEVEL logs_level;
  err = MakeStreamInfo(config_args, false, &sha, &feedback_dir, &data_dir, &logs_level);
  if (err) {
    return;
  }

  if (sha.type == fastotv::VOD_ENCODE || sha.type == fastotv::VOD_RELAY) {
    output_t output;
    if (read_output(config_args, &output)) {
      for (const OutputUri& out_uri : output) {
        auto ouri = out_uri.GetUrl();
        if (ouri.SchemeIsHTTPOrHTTPS()) {
          const auto http_root = out_uri.GetHttpRoot();
          if (http_root) {
            config_args->Insert(CLEANUP_TS_FIELD, common::Value::CreateBooleanValue(false));
            vods_links_.Insert(*http_root, config_args);
          }
        }
      }
    }
  } else if (sha.type == fastotv::COD_ENCODE || sha.type == fastotv::COD_RELAY) {
    output_t output;
    if (read_output(config_args, &output)) {
      for (const OutputUri& out_uri : output) {
        auto ouri = out_uri.GetUrl();
        if (ouri.SchemeIsHTTPOrHTTPS()) {
          const auto http_root = out_uri.GetHttpRoot();
          if (http_root) {
            cods_links_.Insert(*http_root, config_args);
          }
        }
      }
    }
  }
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientActivate(ProtocoledDaemonClient* dclient,
                                                                    const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jactivate = json_tokener_parse(params_ptr);
    if (!jactivate) {
      return common::make_errno_error_inval();
    }

    common::daemon::commands::ActivateInfo activate_info;
    common::Error err_des = activate_info.DeSerialize(jactivate);
    json_object_put(jactivate);
    if (err_des) {
      ignore_result(dclient->ActivateFail(req->id, err_des));
      return common::make_errno_error(err_des->GetDescription(), EAGAIN);
    }

    const auto expire_key = activate_info.GetLicense();
    if (expire_key != config_.license_key) {
      common::Error err = common::make_error("Config not same activation key");
      ignore_result(dclient->ActivateFail(req->id, err));
      return common::make_errno_error(err->GetDescription(), EINVAL);
    }

    common::time64_t tm;
    bool is_valid = common::license::GetExpireTimeFromKey(PROJECT_NAME_LOWERCASE, *expire_key, &tm);
    if (!is_valid) {
      common::Error err = common::make_error("Invalid expire key");
      ignore_result(dclient->ActivateFail(req->id, err));
      return common::make_errno_error(err->GetDescription(), EINVAL);
    }

    const std::string node_stats = MakeServiceStats(tm);
    common::ErrnoError err_ser = dclient->ActivateSuccess(req->id, node_stats);
    if (err_ser) {
      return err_ser;
    }

    dclient->SetVerified(true, tm);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientPingService(ProtocoledDaemonClient* dclient,
                                                                       const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jstop = json_tokener_parse(params_ptr);
    if (!jstop) {
      return common::make_errno_error_inval();
    }

    common::daemon::commands::ClientPingInfo client_ping_info;
    common::Error err_des = client_ping_info.DeSerialize(jstop);
    json_object_put(jstop);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    return dclient->Pong(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientGetLogService(ProtocoledDaemonClient* dclient,
                                                                         const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jlog = json_tokener_parse(params_ptr);
    if (!jlog) {
      return common::make_errno_error_inval();
    }

    common::daemon::commands::GetLogInfo get_log_info;
    common::Error err_des = get_log_info.DeSerialize(jlog);
    json_object_put(jlog);
    if (err_des) {
      ignore_result(dclient->GetLogServiceFail(req->id, err_des));
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    const auto remote_log_path = get_log_info.GetLogPath();
    if (!remote_log_path.SchemeIsHTTPOrHTTPS()) {
      common::ErrnoError errn = common::make_errno_error("Not supported protocol", EAGAIN);
      ignore_result(dclient->GetLogServiceFail(req->id, common::make_error_from_errno(errn)));
      return errn;
    }
    common::Error err =
        common::net::PostHttpFile(common::file_system::ascii_file_string_path(config_.log_path), remote_log_path);
    if (err) {
      ignore_result(dclient->GetLogServiceFail(req->id, err));
      const std::string err_str = err->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    return dclient->GetLogServiceSuccess(req->id);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestServiceCommand(ProtocoledDaemonClient* dclient,
                                                                    const fastotv::protocol::request_t* req) {
  if (req->method == DAEMON_START_STREAM) {
    return HandleRequestClientStartStream(dclient, req);
  } else if (req->method == DAEMON_STOP_STREAM) {
    return HandleRequestClientStopStream(dclient, req);
  } else if (req->method == DAEMON_RESTART_STREAM) {
    return HandleRequestClientRestartStream(dclient, req);
  } else if (req->method == DAEMON_GET_LOG_STREAM) {
    return HandleRequestClientGetLogStream(dclient, req);
  } else if (req->method == DAEMON_GET_PIPELINE_STREAM) {
    return HandleRequestClientGetPipelineStream(dclient, req);
  } else if (req->method == DAEMON_PREPARE_SERVICE) {
    return HandleRequestClientPrepareService(dclient, req);
  } else if (req->method == DAEMON_SYNC_SERVICE) {
    return HandleRequestClientSyncService(dclient, req);
  } else if (req->method == DAEMON_STOP_SERVICE) {
    return HandleRequestClientStopService(dclient, req);
  } else if (req->method == DAEMON_ACTIVATE) {
    return HandleRequestClientActivate(dclient, req);
  } else if (req->method == DAEMON_PING_SERVICE) {
    return HandleRequestClientPingService(dclient, req);
  } else if (req->method == DAEMON_GET_LOG_SERVICE) {
    return HandleRequestClientGetLogService(dclient, req);
  }

  WARNING_LOG() << "Received unknown method: " << req->method;
  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleResponceServiceCommand(ProtocoledDaemonClient* dclient,
                                                                     const fastotv::protocol::response_t* resp) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  fastotv::protocol::request_t req;
  const auto sid = resp->id;
  if (dclient->PopRequestByID(sid, &req)) {
    if (req.method == DAEMON_SERVER_PING) {
      ignore_result(HandleResponcePingService(dclient, resp));
    } else {
      WARNING_LOG() << "HandleResponceServiceCommand not handled responce id: " << *sid << ", command: " << req.method;
    }
  } else {
    WARNING_LOG() << "HandleResponceServiceCommand not found responce id: " << *sid;
  }

  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestStreamsCommand(stream_client_t* pclient,
                                                                    const fastotv::protocol::request_t* req) {
  if (req->method == CHANGED_SOURCES_STREAM) {
    return HandleRequestChangedSourcesStream(pclient, req);
  } else if (req->method == STATISTIC_STREAM) {
    return HandleRequestStatisticStream(pclient, req);
  }
#if defined(MACHINE_LEARNING)
  else if (req->method == ML_NOTIFICATION_STREAM) {
    return HandleRequestMlNotificationStream(pclient, req);
  }
#endif

  WARNING_LOG() << "Received unknown command: " << req->method;
  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleResponceStreamsCommand(stream_client_t* pclient,
                                                                     const fastotv::protocol::response_t* resp) {
  fastotv::protocol::request_t req;
  if (pclient->PopRequestByID(resp->id, &req)) {
    if (req.method == STOP_STREAM) {
    } else if (req.method == RESTART_STREAM) {
    } else {
      WARNING_LOG() << "HandleResponceStreamsCommand not handled command: " << req.method;
    }
  }
  return common::ErrnoError();
}

void ProcessSlaveWrapper::CheckLicenseExpired() {
  const auto license = config_.license_key;
  if (!license) {
    WARNING_LOG() << "You have an invalid license, service stopped";
    StopImpl();
    return;
  }

  common::time64_t tm;
  bool is_valid = common::license::GetExpireTimeFromKey(PROJECT_NAME_LOWERCASE, *license, &tm);
  if (!is_valid) {
    WARNING_LOG() << "You have an invalid license, service stopped";
    StopImpl();
    return;
  }

  if (tm < common::time::current_utc_mstime()) {
    WARNING_LOG() << "Your license have expired, service stopped";
    StopImpl();
    return;
  }
}

std::string ProcessSlaveWrapper::MakeServiceStats(common::time64_t expiration_time) const {
  service::CpuShot next = service::GetMachineCpuShot();
  double cpu_load = service::GetCpuMachineLoad(node_stats_->prev, next);
  node_stats_->prev = next;

  service::NetShot next_nshot = service::GetMachineNetShot();
  uint64_t bytes_recv = (next_nshot.bytes_recv - node_stats_->prev_nshot.bytes_recv);
  uint64_t bytes_send = (next_nshot.bytes_send - node_stats_->prev_nshot.bytes_send);
  node_stats_->prev_nshot = next_nshot;

  service::MemoryShot mem_shot = service::GetMachineMemoryShot();
  service::HddShot hdd_shot = service::GetMachineHddShot();
  service::SysinfoShot sshot = service::GetMachineSysinfoShot();
  std::string uptime_str = common::MemSPrintf("%lu %lu %lu", sshot.loads[0], sshot.loads[1], sshot.loads[2]);
  fastotv::timestamp_t current_time = common::time::current_utc_mstime();
  fastotv::timestamp_t ts_diff = (current_time - node_stats_->timestamp) / 1000;
  if (ts_diff == 0) {
    ts_diff = 1;  // divide by zero
  }
  node_stats_->timestamp = current_time;

  size_t daemons_client_count = 0;
  std::vector<common::libev::IoClient*> clients = loop_->GetClients();
  for (size_t i = 0; i < clients.size(); ++i) {
    ProtocoledDaemonClient* dclient = dynamic_cast<ProtocoledDaemonClient*>(clients[i]);
    if (dclient && dclient->IsVerified()) {
      daemons_client_count++;
    }
  }
  service::OnlineUsers online(daemons_client_count, static_cast<HttpHandler*>(http_handler_)->GetOnlineClients(),
                              static_cast<HttpHandler*>(vods_handler_)->GetOnlineClients(),
                              static_cast<HttpHandler*>(cods_handler_)->GetOnlineClients());
  service::ServerInfo stat(cpu_load, node_stats_->gpu_load, uptime_str, mem_shot.ram_bytes_total,
                           mem_shot.ram_bytes_free, hdd_shot.hdd_bytes_total, hdd_shot.hdd_bytes_free,
                           bytes_recv / ts_diff, bytes_send / ts_diff, sshot.uptime, current_time, online,
                           next_nshot.bytes_recv, next_nshot.bytes_send);

  std::string node_stats;
  if (expiration_time != 0) {
    service::FullServiceInfo fstat(config_.http_host, config_.vods_host, config_.cods_host, expiration_time, stat);
    common::Error err_ser = fstat.SerializeToString(&node_stats);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      WARNING_LOG() << "Failed to generate node full statistic: " << err_str;
    }
  } else {
    common::Error err_ser = stat.SerializeToString(&node_stats);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      WARNING_LOG() << "Failed to generate node statistic: " << err_str;
    }
  }
  return node_stats;
}

}  // namespace server
}  // namespace fastocloud
