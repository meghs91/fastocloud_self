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

#include "stream/elements/sources/tcpsrc.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementTCPSrc::SetHost(const std::string& host) {
  SetProperty("host", host);
}

void ElementTCPSrc::SetPort(uint16_t port) {
  SetProperty("port", port);
}

ElementTCPSrc* make_tcp_server_src(const common::net::HostAndPort& location, element_id_t input_id) {
  ElementTCPSrc* tcpsrc = make_sources<ElementTCPSrc>(input_id);
  tcpsrc->SetHost(location.GetHost());
  tcpsrc->SetPort(location.GetPort());
  return tcpsrc;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
