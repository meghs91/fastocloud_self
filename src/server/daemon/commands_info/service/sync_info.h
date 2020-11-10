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

#include <common/serializer/json_serializer.h>
#include <common/value.h>

#include "base/stream_config.h"

namespace fastocloud {
namespace server {
namespace service {

class SyncInfo : public common::serializer::JsonSerializer<SyncInfo> {
 public:
  typedef JsonSerializer<SyncInfo> base_class;
  typedef StreamConfig config_t;
  typedef std::vector<config_t> streams_t;

  SyncInfo();
  SyncInfo(const streams_t& streams);

  streams_t GetStreams() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  streams_t streams_;
};

}  // namespace service
}  // namespace server
}  // namespace fastocloud
