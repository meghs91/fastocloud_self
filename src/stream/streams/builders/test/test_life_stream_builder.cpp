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

#include "stream/streams/builders/test/test_life_stream_builder.h"

#include "stream/pad/pad.h"  // for Pads

#include "stream/elements/sink/test.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {
namespace test {

TestLifeStreamBuilder::TestLifeStreamBuilder(const RelayConfig* api, SrcDecodeBinStream* observer)
    : RelayStreamBuilder(api, observer) {}

Connector TestLifeStreamBuilder::BuildOutput(Connector conn) {
  return BuildTestOutput(conn);
}

Connector TestLifeStreamBuilder::BuildTestOutput(Connector conn) {
  const RelayConfig* config = static_cast<const RelayConfig*>(GetConfig());
  if (config->HaveVideo()) {
    elements::sink::ElementTestSink* video = elements::sink::make_test_sink(0);
    ElementAdd(video);
    pad::Pad* sink_pad = video->StaticPad("sink");
    if (sink_pad->IsValid()) {
      HandleOutputSinkPadCreated(sink_pad, 0, common::uri::GURL(), false);
    }
    delete sink_pad;
    if (conn.video) {
      ElementLink(conn.video, video);
    }
  }

  if (config->HaveAudio()) {
    elements::sink::ElementTestSink* audio = elements::sink::make_test_sink(1);
    ElementAdd(audio);
    pad::Pad* sink_pad = audio->StaticPad("sink");
    if (sink_pad->IsValid()) {
      HandleOutputSinkPadCreated(sink_pad, 0, common::uri::GURL(), false);
    }
    delete sink_pad;
    if (conn.audio) {
      ElementLink(conn.audio, audio);
    }
  }

  return conn;
}

}  // namespace test
}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
