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

#include "stream/commands_factory.h"

#include <string>

#include "stream_commands/commands.h"

namespace fastocloud {

common::Error ChangedSourcesStreamBroadcast(const ChangedSouresInfo& params, fastotv::protocol::request_t* req) {
  if (!req) {
    return common::make_error_inval();
  }

  std::string req_str;
  common::Error err_ser = params.SerializeToString(&req_str);
  if (err_ser) {
    return err_ser;
  }

  *req = fastotv::protocol::request_t::MakeNotification(CHANGED_SOURCES_STREAM, req_str);
  return common::Error();
}

common::Error StatisticStreamBroadcast(const StatisticInfo& params, fastotv::protocol::request_t* req) {
  if (!req) {
    return common::make_error_inval();
  }

  std::string req_str;
  common::Error err_ser = params.SerializeToString(&req_str);
  if (err_ser) {
    return err_ser;
  }

  *req = fastotv::protocol::request_t::MakeNotification(STATISTIC_STREAM, req_str);
  return common::Error();
}

#if defined(MACHINE_LEARNING)
common::Error NotificationMlStreamBroadcast(const fastotv::commands_info::ml::NotificationInfo& params, fastotv::protocol::request_t* req) {
  if (!req) {
    return common::make_error_inval();
  }

  std::string req_str;
  common::Error err_ser = params.SerializeToString(&req_str);
  if (err_ser) {
    return err_ser;
  }

  *req = fastotv::protocol::request_t::MakeNotification(ML_NOTIFICATION_STREAM, req_str);
  return common::Error();
}
#endif

}  // namespace fastocloud
