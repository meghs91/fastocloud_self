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

#include "stream/streams/builders/mosaic_stream_builder.h"

#include <string.h>
#include <string>

#include "stream/gstreamer_utils.h"  // for pad_get_type

#include "base/constants.h"
#include "base/gst_constants.h"

#include "stream/elements/audio/audio.h"
#include "stream/elements/encoders/audio.h"
#include "stream/elements/encoders/video.h"
#include "stream/elements/muxer/muxer.h"
#include "stream/elements/parser/audio.h"
#include "stream/elements/parser/video.h"
#include "stream/elements/pay/audio.h"
#include "stream/elements/pay/video.h"
#include "stream/elements/sink/screen.h"
#include "stream/elements/sources/build_input.h"
#include "stream/elements/video/video.h"

#include "stream/pad/pad.h"

#include "stream/streams/mosaic_stream.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace {
Element* build_mux_video_scale(common::draw::Size sz, ILinker* linker, Element* link_to, element_id_t video_scale_id) {
  video::ElementVideoScale* videoscale =
      new video::ElementVideoScale(common::MemSPrintf("mux_" VIDEO_SCALE_NAME_1U, video_scale_id));
  ElementCapsFilter* capsfilter =
      new ElementCapsFilter(common::MemSPrintf("mux_" VIDEO_SCALE_CAPS_FILTER_NAME_1U, video_scale_id));
  linker->ElementAdd(videoscale);
  linker->ElementAdd(capsfilter);

  GstCaps* cap_width_height =
      gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, sz.width(), "height", G_TYPE_INT, sz.height(), nullptr);
  capsfilter->SetCaps(cap_width_height);
  gst_caps_unref(cap_width_height);

  linker->ElementLink(link_to, videoscale);
  linker->ElementLink(videoscale, capsfilter);
  return capsfilter;
}
}  // namespace
}  // namespace elements
namespace streams {
namespace builders {

MosaicStreamBuilder::MosaicStreamBuilder(const EncodeConfig* config, MosaicStream* observer)
    : IBaseBuilder(config, observer) {}

bool MosaicStreamBuilder::InitPipeline() {
  const EncodeConfig* config = static_cast<const EncodeConfig*>(GetConfig());
  input_t prepared = config->GetUrl();
  size_t sz = prepared.size();
  MosaicImageOptions options;
  options.screen_size.set_width(1280);
  options.screen_size.set_height(720);
  options.right_padding = 100;
  size_t row_counts = 0;
  size_t column_counts = 0;
  if (sz == 1) {
    row_counts = 1;
    column_counts = 1;
  } else if (sz == 2) {
    row_counts = 2;
    column_counts = 1;
  } else if (sz == 4) {
    row_counts = 2;
    column_counts = 2;
  } else if (sz == 9) {
    row_counts = 3;
    column_counts = 3;
  } else if (sz == 16) {
    row_counts = 4;
    column_counts = 4;
  } else {
    return false;
  }

  elements::video::ElementVideoMixer* vmix =
      new elements::video::ElementVideoMixer(common::MemSPrintf(VIDEOMIXER_NAME_1U, 0));
  ElementAdd(vmix);
  elements::audio::ElementAudioMixer* amix =
      new elements::audio::ElementAudioMixer(common::MemSPrintf(INTERLIVE_NAME_1U, 0));
  ElementAdd(amix);

  size_t i = 0;
  for (size_t r = 0; r < row_counts; ++r) {
    for (size_t c = 0; c < column_counts; ++c) {
      ImageInfo image;
      SoundInfo sound;
      InputUri uri = prepared[i];
      const common::uri::GURL iuri = uri.GetUrl();
      elements::Element* src = elements::sources::make_src(uri, i, IBaseStream::src_timeout_sec);
      pad::Pad* src_pad = src->StaticPad("src");
      if (src_pad->IsValid()) {
        HandleInputSrcPadCreated(src_pad, i, iuri);
      }
      delete src_pad;
      ElementAdd(src);

      elements::ElementDecodebin* decodebin = new elements::ElementDecodebin(common::MemSPrintf(DECODEBIN_NAME_1U, i));
      ElementAdd(decodebin);
      ElementLink(src, decodebin);
      HandleDecodebinCreated(decodebin);

      if (config->HaveVideo()) {
        elements::ElementQueue* video_queue = new elements::ElementQueue(common::MemSPrintf(UDB_VIDEO_NAME_1U, i));
        ElementAdd(video_queue);

        common::draw::Size image_size(options.screen_size.width() / column_counts,
                                      options.screen_size.height() / row_counts);
        image.size = image_size;
        elements::Element* scale = elements::build_mux_video_scale(image.size, this, video_queue, i);

        elements::video::ElementVideoBox* video_box =
            new elements::video::ElementVideoBox(common::MemSPrintf(VIDEO_BOX_NAME_1U, i));
        ElementAdd(video_box);
        ElementLink(scale, video_box);
        video_box->SetProperty("border-alpha", 1.0);
        ElementLink(video_box, vmix);

        const std::string pad_name = common::MemSPrintf("sink_%lu", i);
        pad::Pad* sink_pad = vmix->StaticPad(pad_name.c_str());
        common::draw::Point p(c * image_size.width(), r * image_size.height());
        image.x_y = p;
        if (sink_pad->IsValid()) {
          sink_pad->SetProperty("xpos", p.x());
          sink_pad->SetProperty("ypos", p.y());
        }
        delete sink_pad;
      }

      if (config->HaveAudio()) {
        elements::ElementQueue* audio_queue = new elements::ElementQueue(common::MemSPrintf(UDB_AUDIO_NAME_1U, i));
        ElementAdd(audio_queue);

        elements::audio::ElementLevel* spec =
            new elements::audio::ElementLevel(common::MemSPrintf(AUDIO_LEVEL_NAME_1U, i));
        ElementAdd(spec);
        ElementLink(audio_queue, spec);

        ElementLink(spec, amix);
        /*
        const std::string pad_name = common::MemSPrintf("sink_%lu", i);
        pad::Pad* sink_pad = amix->StaticPad(pad_name.c_str());
        volume_t vol = uri.GetVolume();
        if (sink_pad->IsValid()) {
          if (vol) {
            sink_pad->SetProperty("volume", *vol);
          }
        }
        delete sink_pad;
        sound.volume = vol ? *vol : DEFAULT_VOLUME;
        */
      }
      StreamInfo stream{image, sound};
      options.sreams.push_back(stream);
      i++;
      if (i == sz) {
        break;
      }
    }
  }

  Connector conn{vmix, amix, nullptr};
  if (config->HaveVideo()) {
    elements::video::ElementCairoOverlay* cairo =
        new elements::video::ElementCairoOverlay(common::MemSPrintf(CAIRO_NAME_1U, 0));
    ElementAdd(cairo);
    ElementLink(conn.video, cairo);

    HandleCairoCreated(cairo, options);
    conn.video = cairo;

    elements_line_t first_last = elements::encoders::build_video_convert(config->GetDeinterlace(), this, 0);
    ElementLink(conn.video, first_last.front());
    conn.video = first_last.back();

    elements::ElementTee* tee = new elements::ElementTee(common::MemSPrintf(VIDEO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.video, tee);
    conn.video = tee;
  }
  if (config->HaveAudio()) {
    const auto volume = config->GetVolume();
    const auto achannels = config->GetAudioChannelsCount();
    elements_line_t first_last = elements::encoders::build_audio_converters(volume, achannels, this, 0);
    ElementLink(conn.audio, first_last.front());
    conn.audio = first_last.back();

    elements::ElementTee* tee = new elements::ElementTee(common::MemSPrintf(AUDIO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.audio, tee);
    conn.audio = tee;
  }
  BuildOutput(conn.video, conn.audio);
  return true;
}

void MosaicStreamBuilder::BuildOutput(elements::Element* video, elements::Element* audio) {
  const EncodeConfig* config = static_cast<const EncodeConfig*>(GetConfig());
  output_t out = config->GetOutput();

  for (size_t i = 0; i < out.size(); ++i) {
    const OutputUri output = out[i];

    SinkDeviceType dt;
    if (IsDeviceOutUrl(output.GetUrl(), &dt)) {  // monitor
      if (config->HaveVideo()) {
        elements::ElementQueue* video_tee_queue =
            new elements::ElementQueue(common::MemSPrintf(VIDEO_TEE_QUEUE_NAME_1U, i));
        // video_tee_queue->SetMaxSizeBuffers(4);
        ElementAdd(video_tee_queue);
        elements::Element* next = video_tee_queue;
        ElementLink(video, next);

        elements::Element* video_sink = elements::sink::make_video_device_sink(dt, i);
        if (dt == DECKLINK_OUTPUT) {
          decklink_video_mode_t decklink_mode = config->GetDecklinkMode();
          if (decklink_mode != DEFAULT_DECKLINK_VIDEO_MODE) {
            elements::sink::ElementVideoDeckSink* decl_sink =
                static_cast<elements::sink::ElementVideoDeckSink*>(video_sink);
            decl_sink->SetMode(decklink_mode);
          }
        }
        ElementAdd(video_sink);
        pad::Pad* sink_pad = video_sink->StaticPad("sink");
        if (sink_pad->IsValid()) {
          HandleOutputSinkPadCreated(sink_pad, i, common::uri::GURL(), false);
        }
        delete sink_pad;
        ElementLink(next, video_sink);
      }

      if (config->HaveAudio()) {
        elements::ElementQueue* audio_tee_queue =
            new elements::ElementQueue(common::MemSPrintf(AUDIO_TEE_QUEUE_NAME_1U, i));
        // audio_tee_queue->SetMaxSizeBuffers(4);
        ElementAdd(audio_tee_queue);
        elements::Element* next = audio_tee_queue;
        ElementLink(audio, next);

        elements::Element* audio = elements::sink::make_audio_device_sink(dt, i);
        ElementAdd(audio);
        pad::Pad* sink_pad = audio->StaticPad("sink");
        if (sink_pad->IsValid()) {
          HandleOutputSinkPadCreated(sink_pad, i, common::uri::GURL(), false);
        }
        delete sink_pad;
        ElementLink(next, audio);
      }
      continue;
    }

    common::uri::GURL uri = output.GetUrl();
    bool is_rtp_out = uri.SchemeIsUdp();
    const std::string vcodec = config->GetVideoEncoder();
    elements::Element* mux = elements::muxer::make_muxer(uri, i);
    ElementAdd(mux);

    if (config->HaveVideo()) {
      elements::ElementQueue* video_tee_queue =
          new elements::ElementQueue(common::MemSPrintf(VIDEO_TEE_QUEUE_NAME_1U, i));
      // video_tee_queue->SetMaxSizeBuffers(4);
      ElementAdd(video_tee_queue);
      elements::Element* next = video_tee_queue;
      ElementLink(video, next);

      elements::Element* last = next;
      const auto aratio = config->GetAspectRatio();
      if (aratio) {
        elements::video::ElementAspectRatio* aspect_ratio =
            new elements::video::ElementAspectRatio(common::MemSPrintf(ASPECT_RATIO_NAME_1U, i));
        aspect_ratio->SetAspectRatio(*aratio);
        ElementAdd(aspect_ratio);
        ElementLink(last, aspect_ratio);
        last = aspect_ratio;
      }

      const auto framerate = config->GetFramerate();
      if (framerate && !config->IsMfxGpu()) {
        last = elements::encoders::build_video_framerate(*framerate, this, last, i);
      }

      elements_line_t video_encoder = elements::encoders::build_video_encoder(
          config->GetVideoEncoder(), bit_rate_t(), config->GetVideoEncoderArgs(), config->GetVideoEncoderStrArgs(),
          this, i);
      ElementLink(last, video_encoder.front());
      next = video_encoder.back();

      if (is_rtp_out) {
        elements::Element* rtp_pay = nullptr;
        if (elements::encoders::IsH264Encoder(vcodec)) {
          rtp_pay = elements::pay::make_h264_pay(96, i);
        } else if (vcodec == elements::encoders::ElementX265Enc::GetPluginName()) {
          rtp_pay = elements::pay::make_h265_pay(96, i);
        } else if (vcodec == elements::encoders::ElementMPEG2Enc::GetPluginName()) {
          rtp_pay = elements::pay::make_mpeg2_pay(96, i);
        } else {
          NOTREACHED();
        }
        ElementAdd(rtp_pay);
        ElementLink(next, rtp_pay);
        next = rtp_pay;
      }

#if 0
      if (elements::encoders::IsH264Encoder(vcodec)) {
        elements::parser::ElementH264Parse* premux_parser = elements::parser::make_h264_parser(i);
        ElementAdd(premux_parser);
        ElementLink(next, premux_parser);
        next = premux_parser;
      }
#endif

      ElementLink(next, mux);
    }

    if (config->HaveAudio()) {
      elements::ElementQueue* audio_tee_queue =
          new elements::ElementQueue(common::MemSPrintf(AUDIO_TEE_QUEUE_NAME_1U, i));
      // audio_tee_queue->SetMaxSizeBuffers(4);
      ElementAdd(audio_tee_queue);
      elements::Element* next = audio_tee_queue;
      ElementLink(audio, next);

      const std::string audio_encoder_str = config->GetAudioEncoder();
      std::string name_codec = common::MemSPrintf(AUDIO_CODEC_NAME_1U, i);
      elements::Element* audio_encoder =
          elements::encoders::make_audio_encoder(audio_encoder_str, name_codec, bit_rate_t());
      ElementAdd(audio_encoder);
      if (audio_encoder_str == elements::encoders::ElementMP3Enc::GetPluginName()) {
        elements::audio::ElementAudioResample* audioresample =
            new elements::audio::ElementAudioResample(common::MemSPrintf(AUDIO_RESAMPLE_NAME_1U, i));
        elements::parser::ElementMPEGAudioParse* mpegaudioparse =
            new elements::parser::ElementMPEGAudioParse(common::MemSPrintf(MPEG_AUDIO_PARSE_NAME_1U, i));

        ElementAdd(audioresample);
        ElementAdd(mpegaudioparse);

        ElementLink(next, audioresample);
        ElementLink(audioresample, audio_encoder);
        ElementLink(audio_encoder, mpegaudioparse);
        next = mpegaudioparse;
      } else {
        ElementLink(next, audio_encoder);
        next = audio_encoder;
      }

      if (is_rtp_out) {
        elements::pay::ElementRtpAACPay* rtp_pay = elements::pay::make_aac_pay(97, i);
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
}

void MosaicStreamBuilder::HandleDecodebinCreated(elements::ElementDecodebin* decodebin) {
  MosaicStream* stream = static_cast<MosaicStream*>(GetObserver());
  if (stream) {
    stream->OnDecodebinCreated(decodebin);
  }
}

void MosaicStreamBuilder::HandleCairoCreated(elements::video::ElementCairoOverlay* cairo,
                                             const MosaicImageOptions& options) {
  MosaicStream* stream = static_cast<MosaicStream*>(GetObserver());
  if (stream) {
    stream->OnCairoCreated(cairo, options);
  }
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
