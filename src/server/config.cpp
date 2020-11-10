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

#include "server/config.h"

#include <fstream>
#include <utility>

#include <common/convert2string.h>
#include <common/license/expire_license.h>
#include <common/value.h>

#define SERVICE_LOG_PATH_FIELD "log_path"
#define SERVICE_LOG_LEVEL_FIELD "log_level"
#define SERVICE_HOST_FIELD "host"
#define SERVICE_HTTP_HOST_FIELD "http_host"
#define SERVICE_VODS_HOST_FIELD "vods_host"
#define SERVICE_CODS_HOST_FIELD "cods_host"
#define SERVICE_CODS_TTL_FIELD "cods_ttl"
#define SERVICE_FILES_TTL_FIELD "files_ttl"
#define SERVICE_STREAMLINK_PATH_FIELD "streamlink_path"
#define SERVICE_LICENSE_KEY_FIELD "license_key"

#define DUMMY_LOG_FILE_PATH "/dev/null"

namespace {
std::pair<std::string, std::string> GetKeyValue(const std::string& line, char separator) {
  const size_t pos = line.find(separator);
  if (pos != std::string::npos) {
    const std::string key = line.substr(0, pos);
    const std::string value = line.substr(pos + 1);
    return std::make_pair(key, value);
  }

  return std::make_pair(line, std::string());
}

common::ErrnoError ReadConfigFile(const std::string& path, common::HashValue** args) {
  if (!args) {
    return common::make_errno_error_inval();
  }

  if (path.empty()) {
    return common::make_errno_error("Invalid config path", EINVAL);
  }

  std::ifstream config(path);
  if (!config.is_open()) {
    return common::make_errno_error("Failed to open config file", EINVAL);
  }

  common::HashValue* options = new common::HashValue;
  std::string line;
  while (getline(config, line)) {
    const std::pair<std::string, std::string> pair = GetKeyValue(line, '=');
    if (pair.first == SERVICE_LOG_PATH_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_LOG_LEVEL_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_HOST_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_HTTP_HOST_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_VODS_HOST_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_CODS_HOST_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_CODS_TTL_FIELD) {
      time_t ttl;
      if (common::ConvertFromString(pair.second, &ttl)) {
        options->Insert(pair.first, common::Value::CreateTimeValue(ttl));
      }
    } else if (pair.first == SERVICE_FILES_TTL_FIELD) {
      time_t ttl;
      if (common::ConvertFromString(pair.second, &ttl)) {
        options->Insert(pair.first, common::Value::CreateTimeValue(ttl));
      }
    } else if (pair.first == SERVICE_STREAMLINK_PATH_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    } else if (pair.first == SERVICE_LICENSE_KEY_FIELD) {
      options->Insert(pair.first, common::Value::CreateStringValueFromBasicString(pair.second));
    }
  }

  *args = options;
  return common::ErrnoError();
}

}  // namespace

namespace fastocloud {
namespace server {

Config::Config()
    : host(GetDefaultHost()),
      log_path(DUMMY_LOG_FILE_PATH),
      log_level(common::logging::LOG_LEVEL_INFO),
      cods_ttl(CODS_TTL),
      files_ttl(FILES_TTL),
      streamlink_path(STREAMER_SERVICE_STREAMLINK_PATH),
      license_key() {}

common::net::HostAndPort Config::GetDefaultHost() {
  return common::net::HostAndPort::CreateLocalHostIPV4(CLIENT_PORT);
}

bool Config::IsValid() const {
  return host.IsValid();
}

common::ErrnoError load_config_from_file(const std::string& config_absolute_path, Config* config) {
  if (!config) {
    return common::make_errno_error_inval();
  }

  Config lconfig;
  common::HashValue* slave_config_args = nullptr;
  common::ErrnoError err = ReadConfigFile(config_absolute_path, &slave_config_args);
  if (err) {
    return err;
  }

  common::Value* license_field = slave_config_args->Find(SERVICE_LICENSE_KEY_FIELD);
  std::string license_str;
  if (!license_field || !license_field->GetAsBasicString(&license_str)) {
    return common::make_errno_error(SERVICE_LICENSE_KEY_FIELD " field in config required", EINTR);
  }

  const auto license = common::license::make_license<Config::license_t::value_type>(license_str);
  if (license && common::license::IsValidExpireKey(PROJECT_NAME_LOWERCASE, *license)) {
    lconfig.license_key = license;
  }

  common::Value* log_path_field = slave_config_args->Find(SERVICE_LOG_PATH_FIELD);
  if (!log_path_field || !log_path_field->GetAsBasicString(&lconfig.log_path)) {
    lconfig.log_path = DUMMY_LOG_FILE_PATH;
  }

  std::string level_str;
  common::logging::LOG_LEVEL level = common::logging::LOG_LEVEL_INFO;
  common::Value* level_field = slave_config_args->Find(SERVICE_LOG_LEVEL_FIELD);
  if (level_field && level_field->GetAsBasicString(&level_str)) {
    if (!common::logging::text_to_log_level(level_str.c_str(), &level)) {
      level = common::logging::LOG_LEVEL_INFO;
    }
  }
  lconfig.log_level = level;

  common::Value* host_field = slave_config_args->Find(SERVICE_HOST_FIELD);
  std::string host_str;
  if (!host_field || !host_field->GetAsBasicString(&host_str) || !common::ConvertFromString(host_str, &lconfig.host)) {
    lconfig.host = Config::GetDefaultHost();
  }

  common::Value* http_host_field = slave_config_args->Find(SERVICE_HTTP_HOST_FIELD);
  std::string http_host_str;
  if (!http_host_field || !http_host_field->GetAsBasicString(&http_host_str) ||
      !common::ConvertFromString(http_host_str, &lconfig.http_host)) {
    lconfig.http_host = common::net::HostAndPort::CreateLocalHostIPV4(HTTP_PORT);
  }

  common::Value* vods_host_field = slave_config_args->Find(SERVICE_VODS_HOST_FIELD);
  std::string vods_host_str;
  if (!vods_host_field || !vods_host_field->GetAsBasicString(&vods_host_str) ||
      !common::ConvertFromString(vods_host_str, &lconfig.vods_host)) {
    lconfig.vods_host = common::net::HostAndPort::CreateLocalHostIPV4(VODS_PORT);
  }

  common::Value* cods_host_field = slave_config_args->Find(SERVICE_CODS_HOST_FIELD);
  std::string cods_host_str;
  if (!cods_host_field || !cods_host_field->GetAsBasicString(&cods_host_str) ||
      !common::ConvertFromString(cods_host_str, &lconfig.cods_host)) {
    lconfig.cods_host = common::net::HostAndPort::CreateLocalHostIPV4(CODS_PORT);
  }

  common::Value* cods_ttl_field = slave_config_args->Find(SERVICE_CODS_TTL_FIELD);
  if (!cods_ttl_field || !cods_ttl_field->GetAsTime(&lconfig.cods_ttl)) {
    lconfig.cods_ttl = CODS_TTL;
  }

  common::Value* files_ttl_field = slave_config_args->Find(SERVICE_FILES_TTL_FIELD);
  if (!files_ttl_field || !files_ttl_field->GetAsTime(&lconfig.files_ttl)) {
    lconfig.files_ttl = FILES_TTL;
  }

  common::Value* streamlink_field = slave_config_args->Find(SERVICE_STREAMLINK_PATH_FIELD);
  if (!streamlink_field || !streamlink_field->GetAsBasicString(&lconfig.streamlink_path)) {
    lconfig.streamlink_path = STREAMER_SERVICE_STREAMLINK_PATH;
  }

  *config = lconfig;
  delete slave_config_args;
  return common::ErrnoError();
}

}  // namespace server
}  // namespace fastocloud
