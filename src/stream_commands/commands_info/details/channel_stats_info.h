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

#include <common/serializer/json_serializer.h>

#include "base/channel_stats.h"

namespace fastocloud {
namespace details {

class ChannelStatsInfo : public common::serializer::JsonSerializer<ChannelStatsInfo> {
 public:
  typedef JsonSerializer<ChannelStatsInfo> base_class;
  ChannelStatsInfo();
  explicit ChannelStatsInfo(const ChannelStats& stats);

  ChannelStats GetChannelStats() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  ChannelStats stats_;
};

}  // namespace details
}  // namespace fastocloud
