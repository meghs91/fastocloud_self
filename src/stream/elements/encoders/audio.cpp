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

#include "stream/elements/encoders/audio.h"

#include <string>  // for string
#include <vector>

#include <gst/gstcaps.h>
#include <gst/gstelementfactory.h>

#include "base/constants.h"      // for AUDIO_CODEC, DEFAULT_VOLUME, UDB_...
#include "base/gst_constants.h"  // for VOAAC_ENC

#include "stream/elements/audio/audio.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace encoders {

void ElementFAAC::SetRateControl(guint brtype) {
  SetProperty("rate-control", brtype);
}

void ElementMP3Enc::SetTarget(guint target) {
  SetProperty("target", target);
}

ElementFAAC* make_aac_encoder(element_id_t encoder_id) {
  return make_audio_encoder<ElementFAAC>(encoder_id);
}

ElementVoaacEnc* make_voaac_encoder(element_id_t encoder_id) {
  return make_audio_encoder<ElementVoaacEnc>(encoder_id);
}

ElementFAAC* make_mp3_encoder(element_id_t encoder_id) {
  return make_audio_encoder<ElementFAAC>(encoder_id);
}

Element* make_audio_encoder(const std::string& codec, const std::string& name, bit_rate_t audiorate) {
  if (codec == ElementFAAC::GetPluginName()) {
    ElementFAAC* aac = new ElementFAAC(name);
    if (audiorate) {
      aac->SetRateControl(2);
      aac->SetBitRate(*audiorate * 1024);
    }
    return aac;
  } else if (codec == ElementVoaacEnc::GetPluginName()) {
    ElementVoaacEnc* aac = new ElementVoaacEnc(name);
    if (audiorate) {
      aac->SetBitRate(*audiorate * 1024);
    }
    return aac;
  } else if (codec == ElementMP3Enc::GetPluginName()) {
    ElementMP3Enc* mp3 = new ElementMP3Enc(name);
    if (audiorate) {
      mp3->SetTarget(1);
      mp3->SetBitRate(*audiorate);
    }
    return mp3;
  }

  NOTREACHED() << "Please register new audio encoder type: " << codec;
  return nullptr;
}

Element* make_audio_encoder(const std::string& codec, const std::string& name) {
  if (codec == ElementFAAC::GetPluginName()) {
    ElementFAAC* aac = new ElementFAAC(name);
    return aac;
  } else if (codec == ElementVoaacEnc::GetPluginName()) {
    ElementVoaacEnc* aac = new ElementVoaacEnc(name);
    return aac;
  } else if (codec == ElementMP3Enc::GetPluginName()) {
    ElementMP3Enc* mp3 = new ElementMP3Enc(name);
    return mp3;
  }

  NOTREACHED() << "Please register new audio encoder type: " << codec;
  return nullptr;
}

elements_line_t build_audio_converters(volume_t volume,
                                       audio_channels_count_t achannels,
                                       ILinker* linker,
                                       element_id_t audio_convert_id) {
  elements::audio::ElementAudioConvert* audio_convert =
      new elements::audio::ElementAudioConvert(common::MemSPrintf(AUDIO_CONVERT_NAME_1U, audio_convert_id));
  elements::Element* first = audio_convert;
  elements::Element* last = audio_convert;

  linker->ElementAdd(audio_convert);
  if (achannels) {
    elements::ElementCapsFilter* caps =
        new elements::ElementCapsFilter(common::MemSPrintf(AUDIO_CONVERT_CAPS_FILTER_NAME_1U, audio_convert_id));
    GstCaps* cap_convert = gst_caps_new_simple("audio/x-raw", "channels", G_TYPE_INT, *achannels, nullptr);
    caps->SetCaps(cap_convert);
    gst_caps_unref(cap_convert);

    linker->ElementAdd(caps);
    linker->ElementLink(audio_convert, caps);
    last = caps;
  }

  if (volume) {
    audio::ElementVolume* vol = new audio::ElementVolume(common::MemSPrintf(VOLUME_NAME_1U, audio_convert_id));
    vol->SetVolume(*volume);
    linker->ElementAdd(vol);
    linker->ElementLink(last, vol);
    return {first, vol};
  }

  return {first, last};
}

bool IsAACEncoder(const std::string& encoder) {
  return encoder == ElementFAAC::GetPluginName() || encoder == ElementVoaacEnc::GetPluginName();
}

}  // namespace encoders
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
