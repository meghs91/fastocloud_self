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

#include "stream/elements/sink/build_output.h"

#include <string>

#include "base/output_uri.h"  // for OutputUri, IsFakeUrl

#include "stream/elements/sink/file.h"
#include "stream/elements/sink/http.h"  // for build_http_sink, HlsOutput
#include "stream/elements/sink/rtmp.h"  // for build_rtmp_sink
#include "stream/elements/sink/srt.h"
#include "stream/elements/sink/tcp.h"
#include "stream/elements/sink/udp.h"  // for build_udp_sink

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

Element* build_output(const OutputUri& output, element_id_t sink_id, bool is_vod, bool is_cod) {
  common::uri::GURL uri = output.GetUrl();

  if (uri.SchemeIsUdp()) {
    common::net::HostAndPort host(uri.host(), uri.EffectiveIntPort());
    ElementUDPSink* udp_sink = elements::sink::make_udp_sink(host, sink_id);
    return udp_sink;
  } else if (uri.SchemeIsTcp()) {
    common::net::HostAndPort host(uri.host(), uri.EffectiveIntPort());
    ElementTCPServerSink* tcp_sink = elements::sink::make_tcp_server_sink(host, sink_id);
    return tcp_sink;
  } else if (uri.SchemeIsRtmp()) {
    ElementRtmpSink* rtmp_sink = elements::sink::make_rtmp_sink(sink_id, uri.spec());
    return rtmp_sink;
  } else if (uri.SchemeIsHTTPOrHTTPS()) {
    const auto http_root = output.GetHttpRoot();
    elements::sink::HlsOutput hout;
    common::Error err = is_vod ? MakeVodHlsOutput(uri, http_root, &hout) : MakeHlsOutput(uri, http_root, &hout);
    if (err) {
      NOTREACHED() << err->GetDescription();
      return nullptr;
    }
    ElementHLSSink* http_sink =
        elements::sink::make_http_sink(sink_id, hout, is_cod ? CODS_TS_DURATION : HTTP_TS_DURATION);
    return http_sink;
  } else if (uri.SchemeIsSrt()) {
    int srt_mode = OutputUri::CALLER;
    auto mode = output.GetSrtMode();
    if (mode) {
      srt_mode = *mode;
    }
    ElementSrtSink* srt_sink = elements::sink::make_srt_sink(uri.spec(), srt_mode, sink_id);
    return srt_sink;
  } else if (uri.SchemeIsFile()) {
    ElementFileSink* file_sink = elements::sink::make_file_sink(uri.path(), sink_id);
    return file_sink;
  }

  NOTREACHED() << "Unknown output url: " << uri.spec();
  return nullptr;
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
