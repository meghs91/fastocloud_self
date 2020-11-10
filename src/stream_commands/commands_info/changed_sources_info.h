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

#include "base/input_uri.h"
#include "base/types.h"

namespace fastocloud {

class ChangedSouresInfo : public common::serializer::JsonSerializer<ChangedSouresInfo> {
 public:
  typedef JsonSerializer<ChangedSouresInfo> base_class;
  typedef InputUri url_t;
  ChangedSouresInfo();
  explicit ChangedSouresInfo(const fastotv::stream_id_t& sid, const url_t& url);

  url_t GetUrl() const;
  fastotv::stream_id_t GetStreamID() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  fastotv::stream_id_t id_;
  url_t url_;
};

}  // namespace fastocloud
