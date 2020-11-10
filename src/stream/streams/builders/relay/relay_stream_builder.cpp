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

#include "stream/streams/builders/relay/relay_stream_builder.h"

#include <string>

#include <common/sprintf.h>

#include "stream/elements/encoders/audio.h"
#include "stream/elements/parser/audio.h"
#include "stream/elements/parser/video.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

RelayStreamBuilder::RelayStreamBuilder(const RelayConfig* config, SrcDecodeBinStream* observer)
    : SrcDecodeStreamBuilder(config, observer) {}

Connector RelayStreamBuilder::BuildPostProc(Connector conn) {
  return conn;
}

elements::Element* RelayStreamBuilder::BuildVideoUdbConnection() {
  const RelayConfig* rconfig = static_cast<const RelayConfig*>(GetConfig());
  const std::string video_parser = rconfig->GetVideoParser();
  elements::Element* video = elements::parser::make_video_parser(
      video_parser, common::MemSPrintf(UDB_VIDEO_NAME_1U, 0));  // tsparse, h264parse
  return video;
}

elements::Element* RelayStreamBuilder::BuildAudioUdbConnection() {
  const RelayConfig* rconfig = static_cast<const RelayConfig*>(GetConfig());
  const std::string audio_parser = rconfig->GetAudioParser();
  elements::Element* audio =
      elements::parser::make_audio_parser(audio_parser, common::MemSPrintf(UDB_AUDIO_NAME_1U, 0));
  return audio;
}

Connector RelayStreamBuilder::BuildUdbConnections(Connector conn) {
  CHECK(conn.video == nullptr) << "Must be video empty channel.";
  CHECK(conn.audio == nullptr) << "Must be audio empty channel.";
  const RelayConfig* rconfig = static_cast<const RelayConfig*>(GetConfig());
  if (rconfig->HaveVideo()) {
    elements::Element* vudb = BuildVideoUdbConnection();
    CHECK(vudb);
    ElementAdd(vudb);
    conn.video = vudb;
  }
  if (rconfig->HaveAudio()) {
    elements::Element* audb = BuildAudioUdbConnection();
    CHECK(audb);
    ElementAdd(audb);
    std::string audio_parser = rconfig->GetAudioParser();
    if (audio_parser == elements::parser::ElementRawAudioParse::GetPluginName()) {
      elements::encoders::ElementFAAC* faac = elements::encoders::make_aac_encoder(0);
      ElementAdd(faac);
      ElementLink(audb, faac);
      audb = faac;
    }
    conn.audio = audb;
  }
  return conn;
}

SupportedVideoCodec RelayStreamBuilder::GetVideoCodecType() const {
  const RelayConfig* conf = static_cast<const RelayConfig*>(GetConfig());
  const std::string vparser = conf->GetVideoParser();
  if (vparser == elements::parser::ElementH264Parse::GetPluginName()) {
    return VIDEO_H264_CODEC;
  } else if (vparser == elements::parser::ElementH265Parse::GetPluginName()) {
    return VIDEO_H265_CODEC;
  } else if (vparser == elements::parser::ElementTsParse::GetPluginName()) {
    return VIDEO_MPEG_CODEC;
  }

  NOTREACHED();
  return VIDEO_H264_CODEC;
}

SupportedAudioCodec RelayStreamBuilder::GetAudioCodecType() const {
  const RelayConfig* conf = static_cast<const RelayConfig*>(GetConfig());
  const std::string aparser = conf->GetAudioParser();
  if (aparser == elements::parser::ElementAACParse::GetPluginName()) {
    return AUDIO_AAC_CODEC;
  } else if (aparser == elements::parser::ElementMPEGAudioParse::GetPluginName()) {
    return AUDIO_MPEG_CODEC;
  } else if (aparser == elements::parser::ElementAC3Parse::GetPluginName()) {
    return AUDIO_AC3_CODEC;
  }

  NOTREACHED();
  return AUDIO_MPEG_CODEC;
}

Connector RelayStreamBuilder::BuildConverter(Connector conn) {
  const RelayConfig* config = static_cast<const RelayConfig*>(GetConfig());
  if (config->HaveVideo()) {
    elements::ElementTee* tee = new elements::ElementTee(common::MemSPrintf(VIDEO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.video, tee);
    conn.video = tee;
  }
  if (config->HaveAudio()) {
    elements::ElementTee* tee = new elements::ElementTee(common::MemSPrintf(AUDIO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.audio, tee);
    conn.audio = tee;
  }
  return conn;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
