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

#include "stream/elements/sources/sources.h"  // for ElementLocation

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

class ElementRTSPSrc : public ElementBinEx<ELEMENT_RTSP_SRC> {
 public:
  typedef ElementBinEx<ELEMENT_RTSP_SRC> base_class;
  using base_class::base_class;

  void SetLocation(const std::string& location);  // String; Default: null
  std::string GetLocation() const;

  void SetLatency(gint latency);
};

ElementRTSPSrc* make_rtsp_src(const std::string& location, element_id_t input_id);

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
