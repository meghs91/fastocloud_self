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

#include <string>

#include <common/daemon/client.h>
#include <common/daemon/commands/license_info.h>
#include <common/daemon/commands/ping_info.h>
#include <common/license/types.h>

#include <fastotv/protocol/protocol.h>
#include <fastotv/protocol/types.h>

namespace fastocloud {
namespace server {

class ProtocoledDaemonClient : public fastotv::protocol::ProtocolClient<common::daemon::DaemonClient> {
 public:
  typedef fastotv::protocol::ProtocolClient<common::daemon::DaemonClient> base_class;
  ProtocoledDaemonClient(common::libev::IoLoop* server, const common::net::socket_info& info);

  common::ErrnoError StopMe() WARN_UNUSED_RESULT;

  common::ErrnoError StopFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError StopSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError Ping() WARN_UNUSED_RESULT;
  common::ErrnoError Ping(const common::daemon::commands::ClientPingInfo& server_ping_info) WARN_UNUSED_RESULT;

  common::ErrnoError Pong(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;
  common::ErrnoError Pong(fastotv::protocol::sequance_id_t id,
                          const common::daemon::commands::ServerPingInfo& pong) WARN_UNUSED_RESULT;

  common::ErrnoError ActivateFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError ActivateSuccess(fastotv::protocol::sequance_id_t id, const std::string& result) WARN_UNUSED_RESULT;

  common::ErrnoError PrepareServiceSuccess(fastotv::protocol::sequance_id_t id,
                                           const std::string& result) WARN_UNUSED_RESULT;

  common::ErrnoError GetLogServiceFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError GetLogServiceSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError GetLogStreamFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError GetLogStreamSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError GetPipeStreamFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError GetPipeStreamSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError StartStreamFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError StartStreamSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError ReStartStreamFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError ReStartStreamSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError StopStreamFail(fastotv::protocol::sequance_id_t id, common::Error err) WARN_UNUSED_RESULT;
  common::ErrnoError StopStreamSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  common::ErrnoError SyncServiceSuccess(fastotv::protocol::sequance_id_t id) WARN_UNUSED_RESULT;
};

}  // namespace server
}  // namespace fastocloud
