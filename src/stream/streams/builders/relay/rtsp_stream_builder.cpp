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

#include "stream/streams/builders/relay/rtsp_stream_builder.h"

#include <string>

#include <common/sprintf.h>

#include "base/input_uri.h"  // for InputUri

#include "stream/pad/pad.h"  // for Pad

#include "stream/elements/element.h"  // for Element
#include "stream/elements/sources/rtspsrc.h"

#include "stream/streams/relay/rtsp_relay_stream.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

RtspRelayStreamBuilder::RtspRelayStreamBuilder(const RelayConfig* api, SrcDecodeBinStream* observer)
    : RelayStreamBuilder(api, observer) {}

Connector RtspRelayStreamBuilder::BuildInput() {
  const RelayConfig* config = static_cast<const RelayConfig*>(GetConfig());
  input_t prepared = config->GetUrl();
  InputUri uri = prepared[0];
  const common::uri::GURL url = uri.GetUrl();
  elements::sources::ElementRTSPSrc* src = elements::sources::make_rtsp_src(url.spec(), 0);
  src->SetLatency(0);
  ElementAdd(src);
  HandleRTSPSrcCreated(src);

  elements::ElementDecodebin* decodebin = new elements::ElementDecodebin(common::MemSPrintf(DECODEBIN_NAME_1U, 0));
  ElementAdd(decodebin);
  HandleDecodebinCreated(decodebin);

  return {nullptr, nullptr, nullptr};
}

void RtspRelayStreamBuilder::HandleRTSPSrcCreated(elements::sources::ElementRTSPSrc* src) {
  RtspRelayStream* stream = static_cast<RtspRelayStream*>(GetObserver());
  if (stream) {
    stream->OnRTSPSrcCreated(src);
  }
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
