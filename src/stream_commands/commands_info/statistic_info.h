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

#include "base/stream_struct.h"

namespace fastocloud {

class StatisticInfo : public common::serializer::JsonSerializer<StatisticInfo> {
 public:
  typedef JsonSerializer<StatisticInfo> base_class;
  typedef double cpu_load_t;
  typedef size_t rss_t;

  StatisticInfo();
  StatisticInfo(const StreamStruct& str, cpu_load_t cpu_load, rss_t rss_bytes, fastotv::timestamp_t utc_time);

  StreamStruct GetStreamStruct() const;
  cpu_load_t GetCpuLoad() const;
  rss_t GetRssBytes() const;
  fastotv::timestamp_t GetTimestamp() const;

 protected:
  common::Error SerializeFields(json_object* out) const override;
  common::Error DoDeSerialize(json_object* serialized) override;

 private:
  StreamStruct stream_struct_;
  cpu_load_t cpu_load_;
  rss_t rss_bytes_;
  fastotv::timestamp_t timestamp_;  // utc
};

}  // namespace fastocloud
