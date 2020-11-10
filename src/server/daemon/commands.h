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

#include <fastotv/protocol/types.h>

#if defined(MACHINE_LEARNING)
#include <fastotv/commands_info/ml/notification_info.h>
#endif

#include "server/daemon/commands_info/stream/quit_status_info.h"
#include "stream_commands/commands_info/changed_sources_info.h"
#include "stream_commands/commands_info/statistic_info.h"

// daemon
// client commands

#define DAEMON_START_STREAM "start_stream"  // {"config": {...}, "command_line": {...} }
#define DAEMON_STOP_STREAM "stop_stream"
#define DAEMON_RESTART_STREAM "restart_stream"
#define DAEMON_GET_LOG_STREAM "get_log_stream"
#define DAEMON_GET_PIPELINE_STREAM "get_pipeline_stream"

#define DAEMON_ACTIVATE "activate_request"  // {"key": "XXXXXXXXXXXXXXXXXX"}
#define DAEMON_STOP_SERVICE "stop_service"  // {"delay": 0 }
#define DAEMON_PREPARE_SERVICE \
  "prepare_service"  // { "feedback_directory": "", "timeshifts_directory": "", "hls_directory": "",
                     // "playlists_directory": "", "dvb_directory": "", "capture_card_directory": "" }
#define DAEMON_SYNC_SERVICE "sync_service"
#define DAEMON_PING_SERVICE "ping_service"
#define DAEMON_GET_LOG_SERVICE "get_log_service"  // {"path":"http://localhost/service/id"}

#define DAEMON_SERVER_PING "ping_client"

// Broadcast
#define STREAM_CHANGED_SOURCES_STREAM "changed_source_stream"
#define STREAM_STATISTIC_STREAM "statistic_stream"
#define STREAM_QUIT_STATUS_STREAM "quit_status_stream"
#if defined(MACHINE_LEARNING)
#define STREAM_ML_NOTIFICATION_STREAM "ml_notification_stream"
#endif
#define STREAM_STATISTIC_SERVICE "statistic_service"

namespace fastocloud {
namespace server {

// Broadcast
common::Error ChangedSourcesStreamBroadcast(const ChangedSouresInfo& params, fastotv::protocol::request_t* req);
common::Error StatisitcStreamBroadcast(const StatisticInfo& params, fastotv::protocol::request_t* req);
#if defined(MACHINE_LEARNING)
common::Error MlNotificationStreamBroadcast(const fastotv::commands_info::ml::NotificationInfo& params,
                                            fastotv::protocol::request_t* req);
#endif
common::Error StatisitcServiceBroadcast(fastotv::protocol::serializet_params_t params,
                                        fastotv::protocol::request_t* req);
common::Error QuitStatusStreamBroadcast(const stream::QuitStatusInfo& params, fastotv::protocol::request_t* req);

}  // namespace server
}  // namespace fastocloud
