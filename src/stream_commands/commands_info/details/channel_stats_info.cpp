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

#include "stream_commands/commands_info/details/channel_stats_info.h"

#include <string>

#define FIELD_STATS_ID "id"
#define FIELD_STATS_LAST_UPDATE_TIME "last_update_time"
#define FIELD_STATS_PREV_TOTAL_BYTES "prev_total_bytes"
#define FIELD_STATS_TOTAL_BYTES "total_bytes"
#define FIELD_STATS_BYTES_PER_SECOND "bps"
#define FIELD_STATS_DESIRE_BYTES_PER_SECOND "dbps"

namespace fastocloud {
namespace details {

ChannelStatsInfo::ChannelStatsInfo() : ChannelStatsInfo(ChannelStats()) {}

ChannelStatsInfo::ChannelStatsInfo(const ChannelStats& stats) : stats_(stats) {}

ChannelStats ChannelStatsInfo::GetChannelStats() const {
  return stats_;
}

common::Error ChannelStatsInfo::SerializeFields(json_object* out) const {
  fastotv::channel_id_t sid = stats_.GetID();
  ignore_result(SetIntField(out, FIELD_STATS_ID, sid));

  fastotv::timestamp_t last = stats_.GetLastUpdateTime();
  ignore_result(SetInt64Field(out, FIELD_STATS_LAST_UPDATE_TIME, last));

  size_t prev_t = stats_.GetPrevTotalBytes();
  ignore_result(SetUInt64Field(out, FIELD_STATS_PREV_TOTAL_BYTES, prev_t));

  size_t tot = stats_.GetPrevTotalBytes();
  ignore_result(SetUInt64Field(out, FIELD_STATS_TOTAL_BYTES, tot));

  size_t bps = stats_.GetBps();
  ignore_result(SetUInt64Field(out, FIELD_STATS_BYTES_PER_SECOND, bps));

  common::media::DesireBytesPerSec dbps = stats_.GetDesireBytesPerSecond();
  std::string dbps_str = common::ConvertToString(dbps);
  ignore_result(SetStringField(out, FIELD_STATS_DESIRE_BYTES_PER_SECOND, dbps_str));

  return common::Error();
}

common::Error ChannelStatsInfo::DoDeSerialize(json_object* serialized) {
  int64_t sid;
  common::Error err = GetInt64Field(serialized, FIELD_STATS_ID, &sid);
  if (err) {
    return err;
  }

  ChannelStats stats(sid);
  int64_t lut;
  err = GetInt64Field(serialized, FIELD_STATS_LAST_UPDATE_TIME, &lut);
  if (!err) {
    stats.SetLastUpdateTime(lut);
  }

  int64_t ptb;
  err = GetInt64Field(serialized, FIELD_STATS_PREV_TOTAL_BYTES, &ptb);
  if (!err) {
    stats.SetPrevTotalBytes(ptb);
  }

  int64_t tb;
  err = GetInt64Field(serialized, FIELD_STATS_TOTAL_BYTES, &tb);
  if (!err) {
    stats.SetTotalBytes(tb);
  }

  int64_t bps;
  err = GetInt64Field(serialized, FIELD_STATS_BYTES_PER_SECOND, &bps);
  if (!err) {
    stats.SetBps(bps);
  }

  std::string dbps_str;
  err = GetStringField(serialized, FIELD_STATS_DESIRE_BYTES_PER_SECOND, &dbps_str);
  common::media::DesireBytesPerSec dbps;
  if (!err && common::ConvertFromString(dbps_str, &dbps)) {
    stats.SetDesireBytesPerSecond(dbps);
  }

  *this = ChannelStatsInfo(stats);
  return common::Error();
}

}  // namespace details
}  // namespace fastocloud
