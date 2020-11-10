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
#include <vector>

#include "base/stream_info.h"

namespace fastocloud {

enum StreamStatus { NEW = 0, INIT = 1, STARTED = 2, READY = 3, PLAYING = 4, FROZEN = 5, WAITING = 6 };

struct StreamStruct {
  StreamStruct();
  explicit StreamStruct(const StreamInfo& sha);
  StreamStruct(const StreamInfo& sha, fastotv::timestamp_t start_time, fastotv::timestamp_t lst, size_t rest);
  StreamStruct(fastotv::stream_id_t sid,
               fastotv::StreamType type,
               StreamStatus status,
               input_channels_info_t input,
               output_channels_info_t output,
               fastotv::timestamp_t start_time,
               fastotv::timestamp_t lst,
               size_t rest);

  bool IsValid() const;

  ~StreamStruct();

  void ResetDataWait();

  fastotv::stream_id_t id;
  fastotv::StreamType type;

  fastotv::timestamp_t start_time;
  fastotv::timestamp_t loop_start_time;
  fastotv::timestamp_t idle_time;
  size_t restarts;
  StreamStatus status;

  input_channels_info_t input;
  output_channels_info_t output;
};

}  // namespace fastocloud

namespace common {
std::string ConvertToString(fastocloud::StreamStatus st);
}
