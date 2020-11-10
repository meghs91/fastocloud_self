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

#include "stream/elements/element.h"    // for SupportedElements::ELEMENT_UDP_SINK
#include "stream/elements/sink/sink.h"  // for ElementBaseSink
#include "stream/stypes.h"

namespace common {
struct HostAndPort;
}

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

class ElementUDPSink : public ElementBaseSink<ELEMENT_UDP_SINK> {
 public:
  typedef ElementBaseSink<ELEMENT_UDP_SINK> base_class;
  using base_class::base_class;

  void SetHost(const std::string& host = "localhost");  // String; Default: "localhost"
  void SetPort(uint16_t port = 5004);                   // 0 - 65535; Default: 5004
};

ElementUDPSink* make_udp_sink(const common::net::HostAndPort& host, element_id_t sink_id);

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
