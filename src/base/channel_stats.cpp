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

#include "base/channel_stats.h"

#include <common/time.h>

namespace fastocloud {

ChannelStats::ChannelStats() : ChannelStats(0) {}

ChannelStats::ChannelStats(fastotv::channel_id_t cid)
    : id_(cid),
      last_update_time_(0),
      total_bytes_(0),
      prev_total_bytes_(0),
      bytes_per_second_(0),
      desire_bytes_per_second_() {}

fastotv::channel_id_t ChannelStats::GetID() const {
  return id_;
}

fastotv::timestamp_t ChannelStats::GetLastUpdateTime() const {
  return last_update_time_;
}

void ChannelStats::SetLastUpdateTime(fastotv::timestamp_t t) {
  last_update_time_ = t;
}

size_t ChannelStats::GetTotalBytes() const {
  return total_bytes_;
}

size_t ChannelStats::GetPrevTotalBytes() const {
  return prev_total_bytes_;
}

void ChannelStats::SetPrevTotalBytes(size_t bytes) {
  prev_total_bytes_ = bytes;
}

size_t ChannelStats::GetDiffTotalBytes() const {
  return total_bytes_ - prev_total_bytes_;
}

void ChannelStats::UpdateBps(size_t sec) {
  if (!sec) {
    return;
  }

  bytes_per_second_ = GetDiffTotalBytes() / sec;
}

size_t ChannelStats::GetBps() const {
  return bytes_per_second_;
}

void ChannelStats::SetBps(size_t bps) {
  bytes_per_second_ = bps;
}

void ChannelStats::UpdateCheckPoint() {
  prev_total_bytes_ = total_bytes_;
}

void ChannelStats::SetTotalBytes(size_t bytes) {
  total_bytes_ = bytes;
  last_update_time_ = common::time::current_utc_mstime();
}

void ChannelStats::SetDesireBytesPerSecond(const common::media::DesireBytesPerSec& bps) {
  desire_bytes_per_second_ = bps;
}

common::media::DesireBytesPerSec ChannelStats::GetDesireBytesPerSecond() const {
  return desire_bytes_per_second_;
}

}  // namespace fastocloud
