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

#include "stream/elements/sources/build_input.h"

#include <string>

#include <common/convert2string.h>
#include <common/string_split.h>

#include "stream/elements/sources/dvbsrc.h"
#include "stream/elements/sources/filesrc.h"
#include "stream/elements/sources/httpsrc.h"
#include "stream/elements/sources/rtmpsrc.h"
#include "stream/elements/sources/srtsrc.h"
#include "stream/elements/sources/tcpsrc.h"
#include "stream/elements/sources/udpsrc.h"

namespace {
const char kFFmpegUA[] = "Lavf/58.27.103";
const char kVLC[] = "VLC/3.0.1 LibVLC/3.0.1";
const char kWinkUA[] = "WINK";
const char kChromeUA[] = "Chrome/60.0.3112.113 Safari/537.36";
const char kMozillaUA[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)";
const char kSafariUA[] = "AppleWebKit/537.36 (KHTML, like Gecko)";
}  // namespace

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

Element* make_src(const InputUri& uri, element_id_t input_id, gint timeout_secs) {
  common::uri::GURL url = uri.GetUrl();
  if (url.SchemeIsFile()) {
    return make_file_src(url.path(), input_id);
  } else if (url.SchemeIsHTTPOrHTTPS()) {
    common::Optional<std::string> agent;
    const auto ua = uri.GetUserAgent();
    if (ua) {
      InputUri::UserAgent uagent = *ua;
      if (uagent == InputUri::VLC) {
        agent = std::string(kVLC);
      } else if (uagent == InputUri::FFMPEG) {
        agent = std::string(kFFmpegUA);
      } else if (uagent == InputUri::WINK) {
        agent = std::string(kWinkUA);
      } else if (uagent == InputUri::CHROME) {
        agent = std::string(kChromeUA);
      } else if (uagent == InputUri::MOZILLA) {
        agent = std::string(kMozillaUA);
      } else if (uagent == InputUri::SAFARI) {
        agent = std::string(kSafariUA);
      }
    }

    return make_http_src(url.spec(), agent, uri.GetHttpProxyUrl(), timeout_secs, input_id);
  } else if (url.SchemeIsUdp()) {
    // udp://localhost:8080
    common::net::HostAndPort host(url.host(), url.EffectiveIntPort());
    return make_udp_src(host, uri.GetMulticastIface(), input_id);
  } else if (url.SchemeIsRtmp()) {
    return make_rtmp_src(url.spec(), timeout_secs, input_id);
  } else if (url.SchemeIsTcp()) {
    // tcp://localhost:8080
    common::net::HostAndPort host(url.host(), url.EffectiveIntPort());
    return make_tcp_server_src(host, input_id);
  } else if (url.SchemeIsSrt()) {
    return make_srt_src(url.spec(), input_id);
  } else if (url.SchemeIs("dvb")) {
    // dvb://?modulation=3&trans-mode=1&frequency=514000000
    auto src = make_dvb_src(input_id);
    const auto spl = common::SplitString(url.query(), "&", common::TRIM_WHITESPACE, common::SPLIT_WANT_ALL);
    for (const std::string& line : spl) {
      size_t delem = line.find_first_of('=');
      if (delem == std::string::npos) {
        continue;
      }

      std::string key = line.substr(0, delem);
      std::string value = line.substr(delem + 1);
      gint lmodulation, ltrans;
      guint lfrequency;
      if (key == MODULATION_PROPERTY && common::ConvertFromString(value, &lmodulation)) {
        src->SetModulation(lmodulation);
      } else if (key == TRANS_MODE_PROPERTY && common::ConvertFromString(value, &ltrans)) {
        src->SetTransMode(ltrans);
      } else if (key == FREQUENCY_PROPERTY && common::ConvertFromString(value, &lfrequency)) {
        src->SetFrequency(lfrequency);
      } else if (key == POLARITY_PROPERTY) {
        src->SetPolarity(value);
      }
    }
    return src;
  }

  NOTREACHED() << "Unknown input url: " << url.spec();
  return nullptr;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
