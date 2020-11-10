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

#include "stream/elements/sources/sources.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

class ElementUDPSrc : public ElementPushSrc<ELEMENT_UDP_SRC> {
 public:
  typedef ElementPushSrc<ELEMENT_UDP_SRC> base_class;
  using base_class::base_class;

  void SetAddress(const std::string& host);
  void SetPort(uint16_t port);
  void SetUri(const std::string& uri = "udp://0.0.0.0:5004");  // String. Default: "udp://0.0.0.0:5004"
  void SetMulticastIface(const std::string& iface);            // multicast-iface=10.100.200.10
};

ElementUDPSrc* make_udp_src(const common::net::HostAndPort& host,
                            common::Optional<std::string> iface,
                            element_id_t input_id);

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
