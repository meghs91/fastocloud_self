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

#include "stream/elements/encoders/video.h"

#include <string.h>
#include <string>  // for string

#include <gst/gstelementfactory.h>
#include <gst/gstvalue.h>

#include "base/constants.h"
#include "base/gst_constants.h"

#include "stream/elements/video/video.h"

#define QUEUE_MAX_SIZE_BYTES 1024 * 1024 * 10

// https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-deinterlace.html

namespace fastocloud {
namespace stream {
namespace elements {
namespace encoders {
namespace {

void setup_encoder_params(const video_encoders_args_t& video_args, Element* avc) {
  const std::string video_encoder = avc->GetPluginName();
  for (auto itr = video_args.begin(); itr != video_args.end(); ++itr) {
    const std::string key = itr->first;
    size_t pos = key.find(".");
    std::string prefix = key.substr(0, pos);
    if (prefix != video_encoder) {
      DEBUG_LOG() << "Skipped parametr: " << key << " value: " << itr->second;
      continue;
    }

    std::string param = key.substr(pos + 1);
    DEBUG_LOG() << "Installing parametr: " << key << " value: " << itr->second;
    avc->SetProperty(param.c_str(), itr->second);
  }
}

Element* setup_encoder_str_params(const video_encoders_str_args_t& video_str_args,
                                  ILinker* linker,
                                  Element* avc,
                                  element_id_t encoder_id) {
  Element* last = avc;
  const std::string video_encoder = avc->GetPluginName();
  for (auto it = video_str_args.begin(); it != video_str_args.end(); ++it) {
    const std::string key = it->first;
    size_t pos = key.find(".");
    std::string enc_name = key.substr(0, pos);
    if (enc_name != video_encoder) {
      continue;
    }

    std::string param = key.substr(pos + 1);
    std::string value = it->second;
    DEBUG_LOG() << "Installing parametr for " << enc_name << "." << param << " = " << value;
    if (key == X264_ENC_PARAM("profile")) {
      GstCaps* capsavc = gst_caps_new_simple("video/x-h264", "profile", G_TYPE_STRING, value.c_str(), nullptr);
      ElementCapsFilter* caps =
          new ElementCapsFilter(common::MemSPrintf(VIDEO_X264ENC_CAPS_FILTER_PROFILE_NAME_1U, encoder_id));
      linker->ElementAdd(caps);
      caps->SetCaps(capsavc);
      gst_caps_unref(capsavc);
      linker->ElementLink(last, caps);
      last = caps;
    } else if (key == X264_ENC_PARAM("stream-format")) {
      GstCaps* capsavc = gst_caps_new_simple("video/x-h264", "stream-format", G_TYPE_STRING, value.c_str(), nullptr);
      ElementCapsFilter* caps =
          new ElementCapsFilter(common::MemSPrintf(VIDEO_X264ENC_CAPS_FILTER_STREAM_FORMAT_NAME_1U, encoder_id));
      linker->ElementAdd(caps);
      caps->SetCaps(capsavc);
      gst_caps_unref(capsavc);
      linker->ElementLink(last, caps);
      last = caps;
    } else if (key == X264_ENC_PARAM("tune")) {
      GstCaps* capsavc = gst_caps_new_simple("video/x-h264", "tune", G_TYPE_STRING, value.c_str(), nullptr);
      ElementCapsFilter* caps =
          new ElementCapsFilter(common::MemSPrintf(VIDEO_X264ENC_CAPS_FILTER_TUNE_NAME_1U, encoder_id));
      linker->ElementAdd(caps);
      caps->SetCaps(capsavc);
      gst_caps_unref(capsavc);
      linker->ElementLink(last, caps);
      last = caps;
    } else {
      avc->SetProperty(param.c_str(), value);
    }
  }

  return last;
}

}  // namespace

void ElementMFXH264Enc::SetIDRInterval(guint idr) {
  SetProperty("idr-interval", idr);
}

Element* build_video_scale(int width, int height, ILinker* linker, Element* link_to, element_id_t video_scale_id) {
  video::ElementVideoScale* videoscale =
      new video::ElementVideoScale(common::MemSPrintf(VIDEO_SCALE_NAME_1U, video_scale_id));
  ElementCapsFilter* capsfilter =
      new ElementCapsFilter(common::MemSPrintf(VIDEO_SCALE_CAPS_FILTER_NAME_1U, video_scale_id));
  linker->ElementAdd(videoscale);
  linker->ElementAdd(capsfilter);

  GstCaps* cap_width_height =
      gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, width, "height", G_TYPE_INT, height, nullptr);
  capsfilter->SetCaps(cap_width_height);
  gst_caps_unref(cap_width_height);

  linker->ElementLink(link_to, videoscale);
  linker->ElementLink(videoscale, capsfilter);
  return capsfilter;
}

Element* build_video_framerate(int framerate, ILinker* linker, Element* link_to, element_id_t video_framerate_id) {
  video::ElementVideoRate* videorate =
      new video::ElementVideoRate(common::MemSPrintf(VIDEO_RATE_NAME_1U, video_framerate_id));
  ElementCapsFilter* capsfilter =
      new ElementCapsFilter(common::MemSPrintf(VIDEO_RATE_CAPS_FILTER_NAME_1U, video_framerate_id));
  linker->ElementAdd(videorate);
  linker->ElementAdd(capsfilter);

  GstCaps* cap_framerate = gst_caps_new_simple("video/x-raw", "framerate", GST_TYPE_FRACTION, framerate, 1, nullptr);
  capsfilter->SetCaps(cap_framerate);
  gst_caps_unref(cap_framerate);

  linker->ElementLink(link_to, videorate);
  linker->ElementLink(videorate, capsfilter);
  return capsfilter;
}

ElementX264Enc* make_h264_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementX264Enc>(encoder_id);
}

ElementX265Enc* make_h265_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementX265Enc>(encoder_id);
}

ElementMPEG2Enc* make_mpeg2_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementMPEG2Enc>(encoder_id);
}

ElementEAVCEnc* make_eavc_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementEAVCEnc>(encoder_id);
}

ElementOpenH264Enc* make_openh264_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementOpenH264Enc>(encoder_id);
}

ElementVAAPIH264Enc* make_vaapi_h264_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementVAAPIH264Enc>(encoder_id);
}

ElementVAAPIMpeg2Enc* make_vaapi_mpeg2_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementVAAPIMpeg2Enc>(encoder_id);
}

ElementMFXH264Enc* make_mfx_h264_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementMFXH264Enc>(encoder_id);
}

ElementNvH264Enc* make_nv_h264_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementNvH264Enc>(encoder_id);
}

ElementNvH265Enc* make_nv_h265_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementNvH265Enc>(encoder_id);
}

ElementMsdkH264Enc* make_msdk_h264_encoder(element_id_t encoder_id) {
  return make_video_encoder<ElementMsdkH264Enc>(encoder_id);
}

Element* make_video_encoder(const std::string& codec, const std::string& name) {
  if (codec == ElementX264Enc::GetPluginName()) {
    return new ElementX264Enc(name);
  } else if (codec == ElementX265Enc::GetPluginName()) {
    ElementX265Enc* x265 = new ElementX265Enc(name);
    return x265;
  } else if (codec == ElementMPEG2Enc::GetPluginName()) {
    return new ElementMPEG2Enc(name);
  } else if (codec == ElementVAAPIH264Enc::GetPluginName()) {
    return new ElementVAAPIH264Enc(name);
  } else if (codec == ElementVAAPIMpeg2Enc::GetPluginName()) {
    return new ElementVAAPIMpeg2Enc(name);
  } else if (codec == ElementMFXH264Enc::GetPluginName()) {
    ElementMFXH264Enc* mfx = new ElementMFXH264Enc(name);
    return mfx;
  } else if (codec == ElementOpenH264Enc::GetPluginName()) {
    return new ElementOpenH264Enc(name);
  } else if (codec == ElementEAVCEnc::GetPluginName()) {
    return new ElementEAVCEnc(name);
  } else if (codec == ElementNvH264Enc::GetPluginName()) {
    return new ElementNvH264Enc(name);
  } else if (codec == ElementNvH265Enc::GetPluginName()) {
    return new ElementNvH265Enc(name);
  } else if (codec == ElementMsdkH264Enc::GetPluginName()) {
    return new ElementMsdkH264Enc(name);
  }

  NOTREACHED() << "Please register new video encoder type: " << codec;
  return nullptr;
}

elements_line_t build_video_convert(deinterlace_t deinterlace, ILinker* linker, element_id_t video_convert_id) {
  video::ElementVideoConvert* video_convert =
      new video::ElementVideoConvert(common::MemSPrintf(VIDEO_CONVERT_NAME_1U, video_convert_id));
  linker->ElementAdd(video_convert);
  Element* first = video_convert;
  Element* last = video_convert;

  if (deinterlace && *deinterlace) {
    std::string deinterlace_str = AV_DEINTERLACE;
    Element* deinter =
        video::make_video_deinterlace(deinterlace_str, common::MemSPrintf(DEINTERLACE_NAME_1U, video_convert_id));
    linker->ElementAdd(deinter);
    linker->ElementLink(last, deinter);
    last = deinter;
  }

  return {first, last};
}

elements_line_t build_video_encoder(const std::string& codec,
                                    bit_rate_t video_bitrate,
                                    const video_encoders_args_t& video_args,
                                    const video_encoders_str_args_t& video_str_args,
                                    ILinker* linker,
                                    element_id_t encoder_id) {
  Element* codec_element = make_video_encoder(codec, common::MemSPrintf(VIDEO_CODEC_NAME_1U, encoder_id));
  Element* const first = codec_element;
  linker->ElementAdd(codec_element);

  if (video_bitrate) {
    if (codec_element->GetPluginName() == ElementEAVCEnc::GetPluginName()) {
      codec_element->SetProperty("bitrate-avg", *video_bitrate * 1024);
    } else {
      int bitrate = *video_bitrate;
      if (codec_element->GetPluginName() == ElementOpenH264Enc::GetPluginName()) {
        bitrate *= 1024;
      } else if (codec_element->GetPluginName() == ElementNvH264Enc::GetPluginName()) {
        // bitrate *= 1024;
        codec_element->SetProperty("rc-mode", 2);  // constant
      } else if (codec_element->GetPluginName() == ElementNvH265Enc::GetPluginName()) {
        // bitrate *= 1024;
        codec_element->SetProperty("rc-mode", 1);  // constant
      } else if (codec_element->GetPluginName() == ElementVAAPIH264Enc::GetPluginName()) {
        codec_element->SetProperty("rate-control", 2);  // constant
      } else if (codec_element->GetPluginName() == ElementMFXH264Enc::GetPluginName()) {
        codec_element->SetProperty("rate-control", 1);  // constant
      }

      codec_element->SetProperty("bitrate", bitrate);
    }
  }

  // https://en.wikibooks.org/wiki/MeGUI/x264_Settings
  setup_encoder_params(video_args, codec_element);
  Element* last = setup_encoder_str_params(video_str_args, linker, codec_element, encoder_id);
  return {first, last};
}

bool IsH264Encoder(const std::string& encoder) {
  return encoder == ElementX264Enc::GetPluginName() || encoder == ElementVAAPIH264Enc::GetPluginName() ||
         encoder == ElementOpenH264Enc::GetPluginName() || encoder == ElementNvH264Enc::GetPluginName() ||
         encoder == ElementMFXH264Enc::GetPluginName();
}

}  // namespace encoders
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
