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

#include "stream/elements/sources/udpsrc.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementUDPSrc::SetUri(const std::string& uri) {
  SetProperty("uri", uri);
}

void ElementUDPSrc::SetAddress(const std::string& host) {
  SetProperty("address", host);
}

void ElementUDPSrc::SetPort(uint16_t port) {
  SetProperty("port", port);
}

void ElementUDPSrc::SetMulticastIface(const std::string& iface) {
  SetProperty("multicast-iface", iface);
}

ElementUDPSrc* make_udp_src(const common::net::HostAndPort& host,
                            common::Optional<std::string> iface,
                            element_id_t input_id) {
  ElementUDPSrc* udpsrc = make_sources<ElementUDPSrc>(input_id);
  udpsrc->SetAddress(host.GetHost());
  udpsrc->SetPort(host.GetPort());
  if (iface) {
    udpsrc->SetMulticastIface(*iface);
  }
  return udpsrc;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
