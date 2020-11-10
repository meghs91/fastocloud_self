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

#include "base/stream_struct.h"

#include <string>

#include <common/time.h>

#include "base/channel_stats.h"

namespace common {

std::string ConvertToString(fastocloud::StreamStatus st) {
  static const std::string kStreamStatuses[] = {
      "New", "Inited", "Started", "Ready", "Playing", "Frozen", "Waiting",
  };

  return kStreamStatuses[st];
}

}  // namespace common

namespace fastocloud {

namespace {
output_channels_info_t make_outputs(const std::vector<OutputUri>& output) {
  output_channels_info_t res;
  for (auto out : output) {
    res.push_back(ChannelStats(out.GetID()));
  }
  return res;
}

input_channels_info_t make_inputs(const std::vector<InputUri>& input) {
  input_channels_info_t res;
  for (auto in : input) {
    res.push_back(ChannelStats(in.GetID()));
  }
  return res;
}
}  // namespace

StreamStruct::StreamStruct() : StreamStruct(StreamInfo()) {}

StreamStruct::StreamStruct(const StreamInfo& sha) : StreamStruct(sha, common::time::current_utc_mstime(), 0, 0) {}

StreamStruct::StreamStruct(const StreamInfo& sha,
                           fastotv::timestamp_t start_time,
                           fastotv::timestamp_t lst,
                           size_t rest)
    : StreamStruct(sha.id, sha.type, NEW, make_inputs(sha.input), make_outputs(sha.output), start_time, lst, rest) {}

StreamStruct::StreamStruct(fastotv::stream_id_t sid,
                           fastotv::StreamType type,
                           StreamStatus status,
                           input_channels_info_t input,
                           output_channels_info_t output,
                           fastotv::timestamp_t start_time,
                           fastotv::timestamp_t lst,
                           size_t rest)
    : id(sid),
      type(type),
      start_time(start_time),
      loop_start_time(lst),
      idle_time(0),
      restarts(rest),
      status(status),
      input(input),
      output(output) {}

bool StreamStruct::IsValid() const {
  return !id.empty();
}

StreamStruct::~StreamStruct() {}

void StreamStruct::ResetDataWait() {
  for (size_t i = 0; i < input.size(); ++i) {
    input[i].UpdateCheckPoint();
  }
  for (size_t i = 0; i < output.size(); ++i) {
    output[i].UpdateCheckPoint();
  }
}

}  // namespace fastocloud
