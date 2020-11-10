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

#include "stream/elements/sink/tcp.h"

#include <string>

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

void ElementTCPServerSink::SetHost(const std::string& host) {
  SetProperty("host", host);
}

void ElementTCPServerSink::SetPort(uint16_t port) {
  SetProperty("port", port);
}

ElementTCPServerSink* make_tcp_server_sink(const common::net::HostAndPort& host, element_id_t sink_id) {
  ElementTCPServerSink* tcp_out = make_sink<ElementTCPServerSink>(sink_id);
  tcp_out->SetHost(host.GetHost());
  tcp_out->SetPort(host.GetPort());
  return tcp_out;
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
