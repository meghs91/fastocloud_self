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

#include <vector>

#include "base/channel_stats.h"
#include "base/input_uri.h"
#include "base/output_uri.h"

namespace fastocloud {

typedef std::vector<ChannelStats> input_channels_info_t;
typedef std::vector<ChannelStats> output_channels_info_t;

struct StreamInfo {
  fastotv::stream_id_t id;
  fastotv::StreamType type;
  std::vector<InputUri> input;
  std::vector<OutputUri> output;
};

}  // namespace fastocloud
