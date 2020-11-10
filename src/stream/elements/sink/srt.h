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

#include <common/net/types.h>

// for element_id_t

#include "stream/elements/element.h"
#include "stream/elements/sink/sink.h"
#include "stream/stypes.h"

namespace common {
struct HostAndPort;
}

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

class ElementSrtSink : public ElementBaseSink<ELEMENT_SRT_SINK> {
 public:
  typedef ElementBaseSink<ELEMENT_SRT_SINK> base_class;
  using base_class::base_class;

  void SetUri(const std::string& uri = "srt://127.0.0.1:7001");  // String. Default: "srt://127.0.0.1:7001"
  void SetMode(int mode = 1);
};

ElementSrtSink* make_srt_sink(const std::string& uri, int mode, element_id_t sink_id);

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
