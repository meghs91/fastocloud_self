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

#include <common/media/bandwidth_estimation.h>

#include "base/types.h"

namespace fastocloud {

class ChannelStats {  // only compile time size fields
 public:
  ChannelStats();
  explicit ChannelStats(fastotv::channel_id_t cid);

  fastotv::channel_id_t GetID() const;

  fastotv::timestamp_t GetLastUpdateTime() const;
  void SetLastUpdateTime(fastotv::timestamp_t t);

  size_t GetTotalBytes() const;
  void SetTotalBytes(size_t bytes);

  size_t GetPrevTotalBytes() const;
  void SetPrevTotalBytes(size_t bytes);

  size_t GetDiffTotalBytes() const;

  void UpdateBps(size_t sec);
  size_t GetBps() const;
  void SetBps(size_t bps);

  void UpdateCheckPoint();

  void SetDesireBytesPerSecond(const common::media::DesireBytesPerSec& bps);
  common::media::DesireBytesPerSec GetDesireBytesPerSecond() const;

 private:
  fastotv::channel_id_t id_;

  fastotv::timestamp_t last_update_time_;  // up_time
  size_t total_bytes_;                     // received bytes
  size_t prev_total_bytes_;                // checkpoint received bytes
  size_t bytes_per_second_;                // bps

  common::media::DesireBytesPerSec desire_bytes_per_second_;
};

}  // namespace fastocloud
