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

#include "server/daemon/commands_info/service/server_info.h"

#include <string>

#define STATISTIC_SERVICE_INFO_ONLINE_USERS_FIELD "online_users"

#define OS_FIELD "os"
#define PROJECT_FIELD "project"
#define VERSION_FIELD "version"
#define HTTP_HOST_FIELD "http_host"
#define VODS_HOST_FIELD "vods_host"
#define CODS_HOST_FIELD "cods_host"
#define EXPIRATION_TIME_FIELD "expiration_time"

#define ONLINE_USERS_DAEMON_FIELD "daemon"
#define ONLINE_USERS_HTTP_FIELD "http"
#define ONLINE_USERS_VODS_FIELD "vods"
#define ONLINE_USERS_CODS_FIELD "cods"

namespace fastocloud {
namespace server {
namespace service {

OnlineUsers::OnlineUsers() : OnlineUsers(0, 0, 0, 0) {}

OnlineUsers::OnlineUsers(size_t daemon, size_t http, size_t vods, size_t cods)
    : daemon_(daemon), http_(http), vods_(vods), cods_(cods) {}

common::Error OnlineUsers::DoDeSerialize(json_object* serialized) {
  size_t daemon;
  ignore_result(GetUint64Field(serialized, ONLINE_USERS_DAEMON_FIELD, &daemon));

  size_t http;
  ignore_result(GetUint64Field(serialized, ONLINE_USERS_HTTP_FIELD, &http));

  size_t vods;
  ignore_result(GetUint64Field(serialized, ONLINE_USERS_VODS_FIELD, &vods));

  size_t cods;
  ignore_result(GetUint64Field(serialized, ONLINE_USERS_CODS_FIELD, &cods));

  *this = OnlineUsers(daemon, http, vods, cods);
  return common::Error();
}

common::Error OnlineUsers::SerializeFields(json_object* out) const {
  ignore_result(SetUInt64Field(out, ONLINE_USERS_DAEMON_FIELD, daemon_));
  ignore_result(SetUInt64Field(out, ONLINE_USERS_HTTP_FIELD, http_));
  ignore_result(SetUInt64Field(out, ONLINE_USERS_VODS_FIELD, vods_));
  ignore_result(SetUInt64Field(out, ONLINE_USERS_CODS_FIELD, cods_));
  return common::Error();
}

ServerInfo::ServerInfo() : base_class(), online_users_() {}

ServerInfo::ServerInfo(cpu_load_t cpu_load,
                       gpu_load_t gpu_load,
                       const std::string& load_average,
                       size_t ram_bytes_total,
                       size_t ram_bytes_free,
                       size_t hdd_bytes_total,
                       size_t hdd_bytes_free,
                       fastotv::bandwidth_t net_bytes_recv,
                       fastotv::bandwidth_t net_bytes_send,
                       time_t uptime,
                       fastotv::timestamp_t timestamp,
                       const OnlineUsers& online_users,
                       size_t net_total_bytes_recv,
                       size_t net_total_bytes_send)
    : base_class(cpu_load,
                 gpu_load,
                 load_average,
                 ram_bytes_total,
                 ram_bytes_free,
                 hdd_bytes_total,
                 hdd_bytes_free,
                 net_bytes_recv,
                 net_bytes_send,
                 uptime,
                 timestamp,
                 net_total_bytes_recv,
                 net_total_bytes_send),
      online_users_(online_users) {}

common::Error ServerInfo::SerializeFields(json_object* out) const {
  common::Error err = base_class::SerializeFields(out);
  if (err) {
    return err;
  }

  json_object* obj = nullptr;
  err = online_users_.Serialize(&obj);
  if (err) {
    return err;
  }

  ignore_result(SetObjectField(out, STATISTIC_SERVICE_INFO_ONLINE_USERS_FIELD, obj));
  return common::Error();
}

common::Error ServerInfo::DoDeSerialize(json_object* serialized) {
  ServerInfo inf;
  common::Error err = inf.base_class::DoDeSerialize(serialized);
  if (err) {
    return err;
  }

  json_object* jonline = nullptr;
  json_bool jonline_exists = json_object_object_get_ex(serialized, STATISTIC_SERVICE_INFO_ONLINE_USERS_FIELD, &jonline);
  if (jonline_exists) {
    common::Error err = inf.online_users_.DeSerialize(jonline);
    if (err) {
      return err;
    }
  }

  *this = inf;
  return common::Error();
}

OnlineUsers ServerInfo::GetOnlineUsers() const {
  return online_users_;
}

FullServiceInfo::FullServiceInfo()
    : base_class(),
      http_host_(),
      project_(PROJECT_NAME_LOWERCASE),
      proj_ver_(PROJECT_VERSION_HUMAN),
      os_(fastotv::commands_info::OperationSystemInfo::MakeOSSnapshot()) {}

FullServiceInfo::FullServiceInfo(const common::net::HostAndPort& http_host,
                                 const common::net::HostAndPort& vods_host,
                                 const common::net::HostAndPort& cods_host,
                                 common::time64_t exp_time,
                                 const base_class& base)
    : base_class(base),
      http_host_(http_host),
      vods_host_(vods_host),
      cods_host_(cods_host),
      exp_time_(exp_time),
      project_(PROJECT_NAME_LOWERCASE),
      proj_ver_(PROJECT_VERSION_HUMAN),
      os_(fastotv::commands_info::OperationSystemInfo::MakeOSSnapshot()) {}

common::net::HostAndPort FullServiceInfo::GetHttpHost() const {
  return http_host_;
}

common::net::HostAndPort FullServiceInfo::GetVodsHost() const {
  return vods_host_;
}

common::net::HostAndPort FullServiceInfo::GetCodsHost() const {
  return cods_host_;
}

std::string FullServiceInfo::GetProject() const {
  return project_;
}

std::string FullServiceInfo::GetProjectVersion() const {
  return proj_ver_;
}

common::Error FullServiceInfo::DoDeSerialize(json_object* serialized) {
  FullServiceInfo inf;
  common::Error err = inf.base_class::DoDeSerialize(serialized);
  if (err) {
    return err;
  }

  json_object* jos;
  err = GetObjectField(serialized, OS_FIELD, &jos);
  if (!err) {
    common::Error err = inf.os_.DeSerialize(jos);
    if (err) {
      return err;
    }
  }

  std::string http_host;
  err = GetStringField(serialized, HTTP_HOST_FIELD, &http_host);
  if (!err) {
    common::net::HostAndPort host;
    if (common::ConvertFromString(http_host, &host)) {
      inf.http_host_ = host;
    }
  }

  std::string vods_host;
  err = GetStringField(serialized, VODS_HOST_FIELD, &vods_host);
  if (!err) {
    common::net::HostAndPort host;
    if (common::ConvertFromString(vods_host, &host)) {
      inf.vods_host_ = host;
    }
  }

  std::string cods_host;
  err = GetStringField(serialized, CODS_HOST_FIELD, &cods_host);
  if (!err) {
    common::net::HostAndPort host;
    if (common::ConvertFromString(cods_host, &host)) {
      inf.cods_host_ = host;
    }
  }

  ignore_result(GetInt64Field(serialized, EXPIRATION_TIME_FIELD, &inf.exp_time_));
  ignore_result(GetStringField(serialized, PROJECT_FIELD, &inf.project_));
  ignore_result(GetStringField(serialized, VERSION_FIELD, &inf.proj_ver_));

  *this = inf;
  return common::Error();
}

common::Error FullServiceInfo::SerializeFields(json_object* out) const {
  json_object* jos = nullptr;
  common::Error err = os_.Serialize(&jos);
  if (err) {
    return err;
  }

  std::string http_host_str = common::ConvertToString(http_host_);
  std::string vods_host_str = common::ConvertToString(vods_host_);
  std::string cods_host_str = common::ConvertToString(cods_host_);
  ignore_result(SetStringField(out, HTTP_HOST_FIELD, http_host_str));
  ignore_result(SetStringField(out, VODS_HOST_FIELD, vods_host_str));
  ignore_result(SetStringField(out, CODS_HOST_FIELD, cods_host_str));
  ignore_result(SetInt64Field(out, EXPIRATION_TIME_FIELD, exp_time_));
  ignore_result(SetStringField(out, VERSION_FIELD, proj_ver_));
  ignore_result(SetStringField(out, PROJECT_FIELD, project_));
  ignore_result(SetObjectField(out, OS_FIELD, jos));
  return base_class::SerializeFields(out);
}

}  // namespace service
}  // namespace server
}  // namespace fastocloud
