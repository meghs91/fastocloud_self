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

#include "stream/streams/builders/src_decodebin_stream_builder.h"

#include <string>

#include <common/sprintf.h>

#include "stream/ibase_stream.h"

#include "stream/elements/sources/build_input.h"

#include "stream/streams/src_decodebin_stream.h"

#include "stream/pad/pad.h"

#include "stream/streams/configs/audio_video_config.h"

#include "stream/elements/muxer/muxer.h"
#include "stream/elements/pay/audio.h"
#include "stream/elements/pay/video.h"

namespace fastocloud {
namespace stream {
namespace {
elements::Element* make_video_pay(SupportedVideoCodec vcodec, element_id_t pay_id) {
  if (vcodec == VIDEO_H264_CODEC) {
    return elements::pay::make_h264_pay(96, pay_id);
  } else if (vcodec == VIDEO_H265_CODEC) {
    return elements::pay::make_h265_pay(96, pay_id);
  } else if (vcodec == VIDEO_MPEG_CODEC) {
    return elements::pay::make_mpeg2_pay(96, pay_id);
  }

  NOTREACHED() << "Please add rtp pay for video codec type: " << vcodec;
  return nullptr;
}

elements::Element* make_audio_pay(SupportedAudioCodec acodec, element_id_t pay_id) {
  if (acodec == AUDIO_AAC_CODEC) {
    return elements::pay::make_aac_pay(97, pay_id);
  } else if (acodec == AUDIO_AC3_CODEC) {
    return elements::pay::make_ac3_pay(97, pay_id);
  } else if (acodec == AUDIO_MPEG_CODEC) {
    return elements::pay::make_mpeg2_pay(97, pay_id);
  }

  NOTREACHED() << "Please add rtp pay for audio codec type: " << acodec;
  return nullptr;
}
}  // namespace
namespace streams {
namespace builders {

SrcDecodeStreamBuilder::SrcDecodeStreamBuilder(const AudioVideoConfig* config, SrcDecodeBinStream* observer)
    : GstBaseBuilder(config, observer) {}

Connector SrcDecodeStreamBuilder::BuildInput() {
  elements::Element* src = BuildInputSrc();
  elements::ElementDecodebin* decodebin = new elements::ElementDecodebin(common::MemSPrintf(DECODEBIN_NAME_1U, 0));
  ElementAdd(decodebin);
  ElementLink(src, decodebin);
  HandleDecodebinCreated(decodebin);

  return {nullptr, nullptr, nullptr};
}

void SrcDecodeStreamBuilder::HandleDecodebinCreated(elements::ElementDecodebin* decodebin) {
  SrcDecodeBinStream* stream = static_cast<SrcDecodeBinStream*>(GetObserver());
  if (stream) {
    stream->OnDecodebinCreated(decodebin);
  }
}

elements::Element* SrcDecodeStreamBuilder::BuildInputSrc() {
  const Config* config = GetConfig();
  input_t prepared = config->GetUrl();
  InputUri uri = prepared[0];
  const common::uri::GURL url = uri.GetUrl();
  elements::Element* src = elements::sources::make_src(uri, 0, IBaseStream::src_timeout_sec);
  pad::Pad* src_pad = src->StaticPad("src");
  if (src_pad->IsValid()) {
    HandleInputSrcPadCreated(src_pad, 0, url);
  }
  delete src_pad;
  ElementAdd(src);
  return src;
}

elements::Element* SrcDecodeStreamBuilder::BuildVideoUdbConnection() {
  elements::ElementQueue* video_queue = new elements::ElementQueue(common::MemSPrintf(UDB_VIDEO_NAME_1U, 0));
  return video_queue;
}

elements::Element* SrcDecodeStreamBuilder::BuildAudioUdbConnection() {
  elements::ElementQueue* audio_queue = new elements::ElementQueue(common::MemSPrintf(UDB_AUDIO_NAME_1U, 0));
  return audio_queue;
}

Connector SrcDecodeStreamBuilder::BuildUdbConnections(Connector conn) {
  CHECK(conn.video == nullptr) << "Must be video empty channel.";
  CHECK(conn.audio == nullptr) << "Must be audio empty channel.";
  const AudioVideoConfig* config = static_cast<const AudioVideoConfig*>(GetConfig());
  if (config->HaveVideo()) {
    elements::Element* vudb = BuildVideoUdbConnection();
    CHECK(vudb);
    ElementAdd(vudb);
    conn.video = vudb;
  }
  if (config->HaveAudio()) {
    elements::Element* audb = BuildAudioUdbConnection();
    CHECK(audb);
    ElementAdd(audb);
    conn.audio = audb;
  }
  return conn;
}

Connector SrcDecodeStreamBuilder::BuildOutput(Connector conn) {
  const AudioVideoConfig* config = static_cast<const AudioVideoConfig*>(GetConfig());
  output_t out = config->GetOutput();
  for (size_t i = 0; i < out.size(); ++i) {
    const OutputUri output = out[i];
    SinkDeviceType dt;
    if (IsDeviceOutUrl(output.GetUrl(), &dt)) {  // monitor
      CRITICAL_LOG() << "Decklink not supported for encoding based streams!";
      continue;
    }

    common::uri::GURL uri = output.GetUrl();
    bool is_rtp_out = uri.SchemeIsUdp();
    elements::Element* mux = elements::muxer::make_muxer(uri, i);
    ElementAdd(mux);

    if (config->HaveVideo()) {
      elements::ElementQueue* video_tee_queue =
          new elements::ElementQueue(common::MemSPrintf(VIDEO_TEE_QUEUE_NAME_1U, i));
      ElementAdd(video_tee_queue);
      elements::Element* next = video_tee_queue;
      ElementLink(conn.video, next);

      if (is_rtp_out) {
        elements::Element* rtp_pay = make_video_pay(GetVideoCodecType(), i);
        ElementAdd(rtp_pay);
        ElementLink(next, rtp_pay);
        next = rtp_pay;
      }

      ElementLink(next, mux);
    }

    if (config->HaveAudio()) {
      elements::ElementQueue* audio_tee_queue =
          new elements::ElementQueue(common::MemSPrintf(AUDIO_TEE_QUEUE_NAME_1U, i));
      ElementAdd(audio_tee_queue);
      elements::Element* next = audio_tee_queue;
      ElementLink(conn.audio, next);

      if (is_rtp_out) {
        elements::Element* rtp_pay = make_audio_pay(GetAudioCodecType(), i);
        ElementAdd(rtp_pay);
        ElementLink(next, rtp_pay);
        next = rtp_pay;
      }

      ElementLink(next, mux);
    }

    elements::Element* sink = BuildGenericOutput(output, i);
    ElementAdd(sink);
    ElementLink(mux, sink);
  }
  return conn;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
