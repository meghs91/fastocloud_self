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
    along with fastocloud. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>

#include <common/error.h>
#include <common/value.h>

#include "base/stream_info.h"

namespace fastocloud {

typedef std::shared_ptr<common::HashValue> StreamConfig;

fastotv::stream_id_t GetSid(const StreamConfig& config_args);

common::ErrnoError MakeStreamInfo(const StreamConfig& config_args,
                                  bool check_folders,
                                  StreamInfo* sha,
                                  std::string* feedback_dir,
                                  std::string* data_dir,
                                  common::logging::LOG_LEVEL* logs_level);

}  // namespace fastocloud
