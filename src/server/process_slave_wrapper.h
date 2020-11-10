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

#include <map>
#include <string>
#include <vector>

#include <common/libev/io_loop_observer.h>
#include <common/net/types.h>
#include <common/threads/ts_queue.h>

#include <fastotv/protocol/protocol.h>
#include <fastotv/protocol/types.h>

#include "base/stream_info.h"
#include "base/types.h"

#include "server/base/ihttp_requests_observer.h"
#include "server/config.h"
#include "server/links_holder_ts.h"

namespace fastocloud {
namespace server {

class Child;
class ProtocoledDaemonClient;

class ProcessSlaveWrapper : public common::libev::IoLoopObserver, public server::base::IHttpRequestsObserver {
 public:
  enum {
    node_stats_send_seconds = 10,
    ping_timeout_clients_seconds = 60,
    cleanup_seconds = 3,
    check_license_timeout_seconds = 300
  };
  typedef StreamConfig serialized_stream_t;
  typedef fastotv::protocol::protocol_client_t stream_client_t;

  explicit ProcessSlaveWrapper(const Config& config);
  ~ProcessSlaveWrapper() override;

  static common::ErrnoError SendStopDaemonRequest(const Config& config);
  common::net::HostAndPort GetServerHostAndPort();

  int Exec(int argc, char** argv) WARN_UNUSED_RESULT;

 protected:
  void PreLooped(common::libev::IoLoop* server) override;
  void Accepted(common::libev::IoClient* client) override;
  void Moved(common::libev::IoLoop* server,
             common::libev::IoClient* client) override;  // owner server, now client is orphan
  void Closed(common::libev::IoClient* client) override;
  void TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) override;

  void Accepted(common::libev::IoChild* child) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  void ChildStatusChanged(common::libev::IoChild* child, int status, int signal) override;

  void DataReceived(common::libev::IoClient* client) override;
  void DataReadyToWrite(common::libev::IoClient* client) override;
  void PostLooped(common::libev::IoLoop* server) override;

  void OnHttpRequest(common::libev::http::HttpClient* client,
                     const file_path_t& file,
                     common::http::http_status* recommend_status) override;

  virtual common::ErrnoError HandleRequestServiceCommand(ProtocoledDaemonClient* dclient,
                                                         const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  virtual common::ErrnoError HandleResponceServiceCommand(ProtocoledDaemonClient* dclient,
                                                          const fastotv::protocol::response_t* resp) WARN_UNUSED_RESULT;

  virtual common::ErrnoError HandleRequestStreamsCommand(stream_client_t* pclient,
                                                         const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  virtual common::ErrnoError HandleResponceStreamsCommand(stream_client_t* pclient,
                                                          const fastotv::protocol::response_t* resp) WARN_UNUSED_RESULT;

 private:
  void StopImpl();

  Child* FindChildByID(fastotv::stream_id_t cid) const;
  void BroadcastClients(const fastotv::protocol::request_t& req);

  common::ErrnoError DaemonDataReceived(ProtocoledDaemonClient* dclient) WARN_UNUSED_RESULT;
  common::ErrnoError StreamDataReceived(stream_client_t* pclient) WARN_UNUSED_RESULT;

  common::ErrnoError CreateChildStream(const serialized_stream_t& config_args) WARN_UNUSED_RESULT;
  common::ErrnoError CreateChildStreamImpl(const serialized_stream_t& config_args,
                                           const StreamInfo& sha) WARN_UNUSED_RESULT;
  common::ErrnoError StopChildStream(const serialized_stream_t& config_args);
  common::ErrnoError StopChildStreamImpl(fastotv::stream_id_t sid);

  // stream
  common::ErrnoError HandleRequestChangedSourcesStream(stream_client_t* pclient,
                                                       const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;

  common::ErrnoError HandleRequestStatisticStream(stream_client_t* pclient,
                                                  const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
#if defined(MACHINE_LEARNING)
  common::ErrnoError HandleRequestMlNotificationStream(stream_client_t* pclient,
                                                       const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
#endif

  common::ErrnoError HandleRequestClientStartStream(ProtocoledDaemonClient* dclient,
                                                    const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientStopStream(ProtocoledDaemonClient* dclient,
                                                   const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientRestartStream(ProtocoledDaemonClient* dclient,
                                                      const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientGetLogStream(ProtocoledDaemonClient* dclient,
                                                     const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientGetPipelineStream(ProtocoledDaemonClient* dclient,
                                                          const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;

  // service
  common::ErrnoError HandleRequestClientPrepareService(ProtocoledDaemonClient* dclient,
                                                       const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientSyncService(ProtocoledDaemonClient* dclient,
                                                    const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientActivate(ProtocoledDaemonClient* dclient,
                                                 const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientPingService(ProtocoledDaemonClient* dclient,
                                                    const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientGetLogService(ProtocoledDaemonClient* dclient,
                                                      const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientStopService(ProtocoledDaemonClient* dclient,
                                                    const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;

  common::ErrnoError HandleResponcePingService(ProtocoledDaemonClient* dclient,
                                               const fastotv::protocol::response_t* resp) WARN_UNUSED_RESULT;

  void CheckLicenseExpired();

  std::string MakeServiceStats(common::time64_t expiration_time) const;
  void AddStreamLine(const serialized_stream_t& config_args);

  struct NodeStats;

  const Config config_;

  int process_argc_;
  char** process_argv_;

  common::libev::IoLoop* loop_;
  // http
  common::libev::IoLoop* http_server_;
  common::libev::IoLoopObserver* http_handler_;
  // vods (video on demand)
  common::libev::IoLoop* vods_server_;
  common::libev::IoLoopObserver* vods_handler_;
  // cods (channel on demand)
  common::libev::IoLoop* cods_server_;
  common::libev::IoLoopObserver* cods_handler_;

  common::libev::timer_id_t ping_client_timer_;
  common::libev::timer_id_t check_cods_vods_timer_;
  common::libev::timer_id_t check_old_files_timer_;
  common::libev::timer_id_t node_stats_timer_;
  common::libev::timer_id_t quit_cleanup_timer_;
  common::libev::timer_id_t check_license_timer_;
  NodeStats* node_stats_;

  LinksHolderTS vods_links_;
  LinksHolderTS cods_links_;

  std::vector<common::file_system::ascii_directory_string_path> folders_for_monitor_;
};

}  // namespace server
}  // namespace fastocloud
