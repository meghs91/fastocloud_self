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

#include "stream/streams/builders/test/test_input_stream_builder.h"

#include "stream/elements/element.h"  // for Element
#include "stream/elements/sources/sources.h"

#include "stream/pad/pad.h"  // for Pad

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

TestInputStreamBuilder::TestInputStreamBuilder(const EncodeConfig* api, SrcDecodeBinStream* observer)
    : base_class(api, observer) {}

Connector TestInputStreamBuilder::BuildInput() {
  elements::Element* video = nullptr;
  const EncodeConfig* config = static_cast<const EncodeConfig*>(GetConfig());
  if (config->HaveVideo()) {
    video = new elements::sources::ElementVideoTestSrc("video_src");
    ElementAdd(video);
    pad::Pad* src_pad = video->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(src_pad, 0, common::uri::GURL());
    }
    delete src_pad;
  }

  elements::Element* audio = nullptr;
  if (config->HaveAudio()) {
    audio = new elements::sources::ElementAudioTestSrc("audio_src");
    ElementAdd(audio);
    pad::Pad* src_pad = audio->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(src_pad, 0, common::uri::GURL());
    }
    delete src_pad;
  }
  return {video, audio, nullptr};
}

Connector TestInputStreamBuilder::BuildUdbConnections(Connector conn) {
  return conn;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
