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

// for element_id_t

#include <string>

#include "stream/elements/element.h"    // for SupportedElements::ELEMENT_RTMP_SINK
#include "stream/elements/sink/sink.h"  // for ElementBaseSink
#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

class ElementRtmpSink : public ElementBaseSink<ELEMENT_RTMP_SINK> {
 public:
  typedef ElementBaseSink<ELEMENT_RTMP_SINK> base_class;
  using base_class::base_class;

  void SetLocation(const std::string& location);
};

ElementRtmpSink* make_rtmp_sink(element_id_t sink_id, const std::string& output);

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
