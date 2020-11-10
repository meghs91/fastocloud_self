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

#include "stream/streams/builders/encoding/device_stream_builder.h"

#include <string>

#include <common/sprintf.h>
#include <common/uri/url_parse.h>

#include "base/input_uri.h"  // for InputUri

#include "stream/pad/pad.h"  // for Pad

#include "stream/elements/element.h"  // for Element
#include "stream/elements/sources/alsasrc.h"
#include "stream/elements/sources/v4l2src.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {
namespace encoding {

DeviceStreamBuilder::DeviceStreamBuilder(const EncodeConfig* api, SrcDecodeBinStream* observer)
    : EncodingStreamBuilder(api, observer) {}

Connector DeviceStreamBuilder::BuildInput() {
  const EncodeConfig* config = static_cast<const EncodeConfig*>(GetConfig());
  input_t input = config->GetUrl();
  InputUri diuri = input[0];
  common::uri::GURL duri = diuri.GetUrl();
  elements::Element* video = nullptr;
  if (config->HaveVideo()) {
    elements::sources::ElementV4L2Src* v4l = elements::sources::make_v4l2_src(duri.path(), 0);
    v4l->SetProperty("do-timestamp", true);
    video = v4l;
    ElementAdd(video);
    pad::Pad* src_pad = video->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(src_pad, 0, duri);
    }
    delete src_pad;

    /*elements::ElementCapsFilter* capsfilter =
        new elements::ElementCapsFilter(common::MemSPrintf(VIDEO_CAPS_DEVICE_NAME_1U, 0));
    ElementAdd(capsfilter);

    GstCaps* cap_width_height = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, VIDEO_WIDTH, "height",
                                                    G_TYPE_INT, VIDEO_HEIGHT, nullptr);
    capsfilter->SetCaps(cap_width_height);
    gst_caps_unref(cap_width_height);

    ElementLink(video, capsfilter);
    video = capsfilter;*/

    elements::ElementDecodebin* decodebin = new elements::ElementDecodebin(common::MemSPrintf(DECODEBIN_NAME_1U, 0));
    ElementAdd(decodebin);
    ElementLink(video, decodebin);
    HandleDecodebinCreated(decodebin);
    video = decodebin;
  }

  elements::Element* audio = nullptr;
  if (config->HaveAudio()) {
    const auto query_str = duri.query();
    std::string audio_device;
    common::uri::Component key, value;
    common::uri::Component query(0, query_str.length());
    while (common::uri::ExtractQueryKeyValue(query_str.c_str(), &query, &key, &value)) {
      std::string key_string(query_str.substr(key.begin, key.len));
      std::string param_text(query_str.substr(value.begin, value.len));
      if (common::EqualsASCII(key_string, "audio", false)) {
        audio_device = param_text;
      }
    }

    if (audio_device.empty()) {
      NOTREACHED() << "Invalid audio input.";
    }

    audio = elements::sources::make_alsa_src(audio_device, 1);
    ElementAdd(audio);
    pad::Pad* src_pad = audio->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(src_pad, 1, duri);
    }
    delete src_pad;

    elements::ElementDecodebin* decodebin = new elements::ElementDecodebin(common::MemSPrintf(DECODEBIN_NAME_1U, 1));
    ElementAdd(decodebin);
    ElementLink(audio, decodebin);
    HandleDecodebinCreated(decodebin);
    audio = decodebin;
  }
  return {nullptr, nullptr, nullptr};
}

}  // namespace encoding
}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
