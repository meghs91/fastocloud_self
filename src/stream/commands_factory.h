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

#include <fastotv/protocol/types.h>

#include "stream_commands/commands_info/changed_sources_info.h"
#include "stream_commands/commands_info/statistic_info.h"

#if defined(MACHINE_LEARNING)
#include <fastotv/commands_info/ml/notification_info.h>
#endif

namespace fastocloud {

// Broadcast
common::Error ChangedSourcesStreamBroadcast(const ChangedSouresInfo& params, fastotv::protocol::request_t* req);
common::Error StatisticStreamBroadcast(const StatisticInfo& params, fastotv::protocol::request_t* req);

#if defined(MACHINE_LEARNING)
common::Error NotificationMlStreamBroadcast(const fastotv::commands_info::ml::NotificationInfo& params, fastotv::protocol::request_t* req);
#endif

}  // namespace fastocloud
