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

#include "base/stream_config.h"

#include <string>

#include "base/config_fields.h"
#include "base/inputs_outputs.h"
#include "base/utils.h"

namespace fastocloud {

fastotv::stream_id_t GetSid(const StreamConfig& config_args) {
  common::Value* id_field = config_args->Find(ID_FIELD);
  fastotv::stream_id_t sid;
  if (id_field && id_field->GetAsBasicString(&sid)) {
    return sid;
  }

  return sid;
}

common::ErrnoError MakeStreamInfo(const StreamConfig& config_args,
                                  bool check_folders,
                                  StreamInfo* sha,
                                  std::string* feedback_dir,
                                  std::string* data_dir,
                                  common::logging::LOG_LEVEL* logs_level) {
  if (!sha || !data_dir || !feedback_dir || !logs_level) {
    return common::make_errno_error_inval();
  }

  StreamInfo lsha;
  common::Value* id_field = config_args->Find(ID_FIELD);
  if (!id_field || !id_field->GetAsBasicString(&lsha.id)) {
    return common::make_errno_error("Define " ID_FIELD " variable and make it valid", EAGAIN);
  }

  int type;
  common::Value* type_field = config_args->Find(TYPE_FIELD);
  if (!type_field || !type_field->GetAsInteger(&type)) {
    return common::make_errno_error("Define " TYPE_FIELD " variable and make it valid", EAGAIN);
  }
  lsha.type = static_cast<fastotv::StreamType>(type);

  if (lsha.type == fastotv::PROXY || lsha.type == fastotv::VOD_PROXY) {
    return common::make_errno_error("Proxy streams not handled for now", EINVAL);
  }

  std::string lfeedback_dir;
  common::Value* feedback_field = config_args->Find(FEEDBACK_DIR_FIELD);
  if (!feedback_field || !feedback_field->GetAsBasicString(&lfeedback_dir)) {
    return common::make_errno_error("Define " FEEDBACK_DIR_FIELD " variable and make it valid", EAGAIN);
  }

  std::string ldata_dir;
  common::Value* data_field = config_args->Find(DATA_DIR_FIELD);
  if (!data_field || !data_field->GetAsBasicString(&ldata_dir)) {
    return common::make_errno_error("Define " DATA_DIR_FIELD " variable and make it valid", EAGAIN);
  }

  int llogs_level;
  common::Value* log_level_field = config_args->Find(LOG_LEVEL_FIELD);
  if (!log_level_field || !log_level_field->GetAsInteger(&llogs_level)) {
    llogs_level = common::logging::LOG_LEVEL_DEBUG;
  }

  common::ErrnoError errn = CreateAndCheckDir(lfeedback_dir);
  if (errn) {
    return errn;
  }

  errn = CreateAndCheckDir(ldata_dir);
  if (errn) {
    return errn;
  }

  if (!read_input(config_args, &lsha.input)) {
    return common::make_errno_error("Define " INPUT_FIELD " variable and make it valid", EAGAIN);
  }

  if (check_folders) {
    bool is_timeshift_rec_or_catchup = type == fastotv::TIMESHIFT_RECORDER || type == fastotv::CATCHUP;  // no outputs
    if (is_timeshift_rec_or_catchup) {
      std::string timeshift_dir;
      common::Value* timeshift_dir_field = config_args->Find(TIMESHIFT_DIR_FIELD);
      if (!timeshift_dir_field || !timeshift_dir_field->GetAsBasicString(&timeshift_dir)) {
        return common::make_errno_error("Define " TIMESHIFT_DIR_FIELD " variable and make it valid", EAGAIN);
      }

      errn = CreateAndCheckDir(timeshift_dir);
      if (errn) {
        return errn;
      }
    } else {
      output_t output;
      if (!read_output(config_args, &output)) {
        return common::make_errno_error("Define " OUTPUT_FIELD " variable and make it valid", EAGAIN);
      }

      for (auto out_uri : output) {
        auto ouri = out_uri.GetUrl();
        if (ouri.SchemeIsHTTPOrHTTPS()) {
          const auto http_root = out_uri.GetHttpRoot();
          if (!http_root) {
            return common::make_errno_error_inval();
          }

          const std::string http_root_str = http_root->GetPath();
          common::ErrnoError errn = CreateAndCheckDir(http_root_str);
          if (errn) {
            return errn;
          }
        } else if (ouri.SchemeIsFile()) {
          const auto file_path = common::file_system::ascii_file_string_path(ouri.path());
          if (!file_path.IsValid()) {
            return common::make_errno_error_inval();
          }

          const std::string file_root_str = file_path.GetDirectory();
          common::ErrnoError errn = CreateAndCheckDir(file_root_str);
          if (errn) {
            return errn;
          }
        }
        lsha.output.push_back(out_uri);
      }
    }
  }

  *logs_level = static_cast<common::logging::LOG_LEVEL>(llogs_level);
  *feedback_dir = lfeedback_dir;
  *data_dir = ldata_dir;
  *sha = lsha;
  return common::ErrnoError();
}

}  // namespace fastocloud
