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

#include "stream_commands/commands_info/statistic_info.h"

#include <math.h>

#include "stream_commands/commands_info/details/channel_stats_info.h"

#define STREAM_ID_FIELD "id"
#define STREAM_TYPE_FIELD "type"
#define STREAM_CPU_FIELD "cpu"
#define STREAM_RSS_FIELD "rss"
#define STREAM_STATUS_FIELD "status"
#define STREAM_LOOP_START_TIME_FIELD "loop_start_time"
#define STREAM_RESTARTS_FIELD "restarts"
#define STREAM_START_TIME_FIELD "start_time"
#define STREAM_TIMESTAMP_FIELD "timestamp"
#define STREAM_IDLE_TIME_FIELD "idle_time"

#define STREAM_INPUT_STREAMS_FIELD "input_streams"
#define STREAM_OUTPUT_STREAMS_FIELD "output_streams"

namespace fastocloud {

StatisticInfo::StatisticInfo() : stream_struct_(), cpu_load_(), rss_bytes_(), timestamp_(0) {}

StatisticInfo::StatisticInfo(const StreamStruct& str,
                             cpu_load_t cpu_load,
                             rss_t rss_bytes,
                             fastotv::timestamp_t utc_time)
    : stream_struct_(str), cpu_load_(cpu_load), rss_bytes_(rss_bytes), timestamp_(utc_time) {}

StreamStruct StatisticInfo::GetStreamStruct() const {
  return stream_struct_;
}

StatisticInfo::cpu_load_t StatisticInfo::GetCpuLoad() const {
  return cpu_load_;
}

StatisticInfo::rss_t StatisticInfo::GetRssBytes() const {
  return rss_bytes_;
}

fastotv::timestamp_t StatisticInfo::GetTimestamp() const {
  return timestamp_;
}

common::Error StatisticInfo::SerializeFields(json_object* out) const {
  if (!stream_struct_.IsValid()) {
    return common::make_error_inval();
  }

  const auto channel_id = stream_struct_.id;
  ignore_result(SetStringField(out, STREAM_ID_FIELD, channel_id));
  ignore_result(SetEnumField(out, STREAM_TYPE_FIELD, stream_struct_.type));

  input_channels_info_t input_streams = stream_struct_.input;
  json_object* jinput_streams = json_object_new_array();
  for (auto inf : input_streams) {
    json_object* jinf = nullptr;
    details::ChannelStatsInfo sinf(inf);
    common::Error err = sinf.Serialize(&jinf);
    if (err) {
      continue;
    }
    json_object_array_add(jinput_streams, jinf);
  }
  ignore_result(SetArrayField(out, STREAM_INPUT_STREAMS_FIELD, jinput_streams));

  output_channels_info_t output_streams = stream_struct_.output;
  json_object* joutput_streams = json_object_new_array();
  for (auto inf : output_streams) {
    json_object* jinf = nullptr;
    details::ChannelStatsInfo sinf(inf);
    common::Error err = sinf.Serialize(&jinf);
    if (err) {
      continue;
    }
    json_object_array_add(joutput_streams, jinf);
  }
  ignore_result(SetArrayField(out, STREAM_OUTPUT_STREAMS_FIELD, joutput_streams));

  ignore_result(SetInt64Field(out, STREAM_LOOP_START_TIME_FIELD, stream_struct_.loop_start_time));
  ignore_result(SetUInt64Field(out, STREAM_RSS_FIELD, rss_bytes_));
  ignore_result(SetDoubleField(out, STREAM_CPU_FIELD, cpu_load_));
  ignore_result(SetEnumField(out, STREAM_STATUS_FIELD, stream_struct_.status));
  ignore_result(SetUInt64Field(out, STREAM_RESTARTS_FIELD, stream_struct_.restarts));
  ignore_result(SetInt64Field(out, STREAM_START_TIME_FIELD, stream_struct_.start_time));
  ignore_result(SetInt64Field(out, STREAM_TIMESTAMP_FIELD, timestamp_));
  ignore_result(SetInt64Field(out, STREAM_IDLE_TIME_FIELD, stream_struct_.idle_time));
  return common::Error();
}

common::Error StatisticInfo::DoDeSerialize(json_object* serialized) {
  fastotv::stream_id_t cid;
  common::Error err = GetStringField(serialized, STREAM_ID_FIELD, &cid);
  if (err) {
    return err;
  }

  fastotv::StreamType type;
  err = GetEnumField<fastotv::StreamType>(serialized, STREAM_TYPE_FIELD, &type);
  if (err) {
    return err;
  }

  input_channels_info_t input;
  json_object* jinput = nullptr;
  size_t len;
  err = GetArrayField(serialized, STREAM_INPUT_STREAMS_FIELD, &jinput, &len);
  if (!err) {
    for (size_t i = 0; i < len; ++i) {
      json_object* jin = json_object_array_get_idx(jinput, i);
      details::ChannelStatsInfo sinf;
      common::Error err = sinf.DeSerialize(jin);
      if (err) {
        continue;
      }

      input.push_back(ChannelStats(sinf.GetChannelStats()));
    }
  }

  output_channels_info_t output;
  json_object* joutput = nullptr;
  err = GetArrayField(serialized, STREAM_OUTPUT_STREAMS_FIELD, &joutput, &len);
  if (!err) {
    for (size_t i = 0; i < len; ++i) {
      json_object* jin = json_object_array_get_idx(joutput, i);
      details::ChannelStatsInfo sinf;
      common::Error err = sinf.DeSerialize(jin);
      if (err) {
        continue;
      }

      output.push_back(ChannelStats(sinf.GetChannelStats()));
    }
  }

  StreamStatus st = NEW;
  ignore_result(GetEnumField(serialized, STREAM_STATUS_FIELD, &st));

  cpu_load_t cpu_load = 0;
  ignore_result(GetDoubleField(serialized, STREAM_CPU_FIELD, &cpu_load));

  rss_t rss = 0;
  ignore_result(GetUint64Field(serialized, STREAM_RSS_FIELD, &rss));

  fastotv::timestamp_t time = 0;
  ignore_result(GetInt64Field(serialized, STREAM_TIMESTAMP_FIELD, &time));

  fastotv::timestamp_t start_time = 0;
  ignore_result(GetInt64Field(serialized, STREAM_START_TIME_FIELD, &start_time));

  size_t restarts = 0;
  ignore_result(GetUint64Field(serialized, STREAM_RESTARTS_FIELD, &restarts));

  fastotv::timestamp_t loop_start_time = 0;
  ignore_result(GetInt64Field(serialized, STREAM_LOOP_START_TIME_FIELD, &loop_start_time));

  fastotv::timestamp_t idle_time = 0;
  ignore_result(GetInt64Field(serialized, STREAM_IDLE_TIME_FIELD, &idle_time));

  StreamStruct strct(cid, type, st, input, output, start_time, loop_start_time, restarts);
  strct.idle_time = idle_time;
  *this = StatisticInfo(strct, cpu_load, rss, time);
  return common::Error();
}

}  // namespace fastocloud
