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

#include "stream/streams/builders/relay/playlist_relay_stream_builder.h"

#include "stream/pad/pad.h"

#include "stream/elements/sources/appsrc.h"

#include "stream/streams/relay/playlist_relay_stream.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

PlaylistRelayStreamBuilder::PlaylistRelayStreamBuilder(const PlaylistRelayConfig* api, PlaylistRelayStream* observer)
    : RelayStreamBuilder(api, observer) {}

void PlaylistRelayStreamBuilder::HandleAppSrcCreated(elements::sources::ElementAppSrc* src) {
  PlaylistRelayStream* stream = static_cast<PlaylistRelayStream*>(GetObserver());
  if (stream) {
    stream->OnAppSrcCreatedCreated(src);
  }
}

elements::Element* PlaylistRelayStreamBuilder::BuildInputSrc() {
  elements::sources::ElementAppSrc* appsrc = elements::sources::make_app_src(0);
  // g_signal_connect(appsrc, "enough-data", G_CALLBACK(enough_data), this);

  pad::Pad* src_pad = appsrc->StaticPad("src");
  if (src_pad->IsValid()) {
    HandleInputSrcPadCreated(src_pad, 0, common::uri::GURL());
  }
  delete src_pad;
  ElementAdd(appsrc);
  HandleAppSrcCreated(appsrc);
  return appsrc;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
