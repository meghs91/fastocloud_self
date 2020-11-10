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

#pragma once

#include <string>  // for string

#include <common/sprintf.h>

#include "base/types.h"

#include "stream/gst_types.h"
#include "stream/ilinker.h"  // for ILinker (ptr only), elements_line_t
#include "stream/stypes.h"
// for element_id_t, AUDIO_CODEC_NAME_1U

#include "stream/elements/element.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace encoders {

template <SupportedElements el>
class ElementAudioEncoder : public ElementEx<el> {
 public:
  typedef ElementEx<el> base_class;
  using base_class::base_class;

  void SetBitRate(gint bitrate) { ElementEx<el>::SetProperty("bitrate", bitrate); }
};

class ElementFAAC : public ElementAudioEncoder<ELEMENT_FAAC> {
 public:
  typedef ElementAudioEncoder<ELEMENT_FAAC> base_class;
  using base_class::base_class;

  void SetRateControl(guint brtype = 1);  // Default value: VBR (1)
                                          // Allowed values: VBR (1), ABR (2)
};

class ElementVoaacEnc : public ElementAudioEncoder<ELEMENT_VOAAC_ENC> {
 public:
  typedef ElementAudioEncoder<ELEMENT_VOAAC_ENC> base_class;
  using base_class::base_class;
};

class ElementMP3Enc : public ElementAudioEncoder<ELEMENT_LAME_MP3_ENC> {
 public:
  typedef ElementAudioEncoder<ELEMENT_LAME_MP3_ENC> base_class;
  using base_class::base_class;

  void SetTarget(guint target = 0);  // Default: 0, "quality"
                                     // (0): quality          - Quality
                                     // (1): bitrate          - Bitrate
};

template <typename T>
T* make_audio_encoder(element_id_t encoder_id) {
  return make_element<T>(common::MemSPrintf(AUDIO_CODEC_NAME_1U, encoder_id));
}

ElementFAAC* make_aac_encoder(element_id_t encoder_id);
ElementVoaacEnc* make_voaac_encoder(element_id_t encoder_id);
ElementFAAC* make_mp3_encoder(element_id_t encoder_id);

Element* make_audio_encoder(const std::string& codec, const std::string& name, bit_rate_t audiorate);
Element* make_audio_encoder(const std::string& codec, const std::string& name);

elements_line_t build_audio_converters(volume_t volume,
                                       audio_channels_count_t achannels,
                                       ILinker* linker,
                                       element_id_t audio_convert_id);

bool IsAACEncoder(const std::string& encoder);

}  // namespace encoders
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
