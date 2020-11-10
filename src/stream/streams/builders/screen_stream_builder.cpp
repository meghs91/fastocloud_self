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

#include "stream/streams/builders/screen_stream_builder.h"

#include "stream/elements/element.h"  // for Element
#include "stream/elements/sink/screen.h"
#include "stream/elements/sources/sources.h"

#include "stream/pad/pad.h"

#define VIDEO_SRC_NAME_1U "video_src_%lu"
#define AUDIO_SRC_NAME_1U "audio_src_%lu"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

ScreenStreamBuilder::ScreenStreamBuilder(const AudioVideoConfig* api, IBaseBuilderObserver* observer)
    : GstBaseBuilder(api, observer) {}

Connector ScreenStreamBuilder::BuildInput() {
  const AudioVideoConfig* config = static_cast<const AudioVideoConfig*>(GetConfig());
  elements::Element* video = nullptr;
  if (config->HaveVideo()) {
    video = new elements::sources::ElementVideoTestSrc(common::MemSPrintf(VIDEO_SRC_NAME_1U, 0));
    ElementAdd(video);
    pad::Pad* src_pad = video->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(src_pad, 0, common::uri::GURL());
    }
    delete src_pad;
  }

  elements::Element* audio = nullptr;
  if (config->HaveAudio()) {
    audio = new elements::sources::ElementAudioTestSrc(common::MemSPrintf(AUDIO_SRC_NAME_1U, 0));
    ElementAdd(audio);
    pad::Pad* src_pad = audio->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(src_pad, 0, common::uri::GURL());
    }
    delete src_pad;
  }
  return {video, audio, nullptr};
}

Connector ScreenStreamBuilder::BuildUdbConnections(Connector conn) {
  return conn;
}

Connector ScreenStreamBuilder::BuildPostProc(Connector conn) {
  return conn;
}

Connector ScreenStreamBuilder::BuildConverter(Connector conn) {
  return conn;
}

Connector ScreenStreamBuilder::BuildOutput(Connector conn) {
  const AudioVideoConfig* aconf = static_cast<const AudioVideoConfig*>(GetConfig());
  output_t out = aconf->GetOutput();
  for (size_t i = 0; i < out.size(); ++i) {
    if (aconf->HaveVideo()) {
      elements::Element* video = elements::sink::make_video_screen_sink(i);
      ElementAdd(video);
      pad::Pad* sink_pad = video->StaticPad("sink");
      if (sink_pad->IsValid()) {
        HandleOutputSinkPadCreated(sink_pad, i, common::uri::GURL(), false);
      }
      delete sink_pad;
      if (conn.video) {
        ElementLink(conn.video, video);
      }
    }

    if (aconf->HaveAudio()) {
      elements::Element* audio = elements::sink::make_audio_screen_sink(i + 1);
      ElementAdd(audio);
      pad::Pad* sink_pad = audio->StaticPad("sink");
      if (sink_pad->IsValid()) {
        HandleOutputSinkPadCreated(sink_pad, i, common::uri::GURL(), false);
      }
      delete sink_pad;
      if (conn.audio) {
        ElementLink(conn.audio, audio);
      }
    }
  }

  return conn;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
