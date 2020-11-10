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

#include "stream/elements/element.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace audio {

class ElementAudioConvert : public ElementEx<ELEMENT_AUDIO_CONVERT> {
 public:
  typedef ElementEx<ELEMENT_AUDIO_CONVERT> base_class;
  using base_class::base_class;
};

class ElementAudioResample : public ElementEx<ELEMENT_AUDIO_RESAMPLE> {
 public:
  typedef ElementEx<ELEMENT_AUDIO_RESAMPLE> base_class;
  using base_class::base_class;
};

class ElementVolume : public ElementEx<ELEMENT_VOLUME> {
 public:
  typedef ElementEx<ELEMENT_VOLUME> base_class;
  using base_class::base_class;

  void SetVolume(gdouble volume = 1);  // 0 - 10 Default: 1
};

class ElementRGVolume : public ElementEx<ELEMENT_RG_VOLUME> {
 public:
  typedef ElementEx<ELEMENT_RG_VOLUME> base_class;
  using base_class::base_class;

  void SetPreAmp(gdouble volume = 0);  // -60 - 60 Default: 0
};

class ElementAudioMixer : public ElementEx<ELEMENT_AUDIO_MIXER> {
 public:
  typedef ElementEx<ELEMENT_AUDIO_MIXER> base_class;
  using base_class::base_class;
};

class ElementIterleave : public ElementEx<ELEMENT_INTERLEAVE> {
 public:
  typedef ElementEx<ELEMENT_INTERLEAVE> base_class;
  using base_class::base_class;
};

class ElementDeinterleave : public ElementEx<ELEMENT_DEINTERLEAVE> {
 public:
  typedef ElementEx<ELEMENT_DEINTERLEAVE> base_class;
  using base_class::base_class;
};

class ElementSpectrum : public ElementEx<ELEMENT_SPECTRUM> {
 public:
  typedef ElementEx<ELEMENT_SPECTRUM> base_class;
  using base_class::base_class;

  void SetBands(guint bands = 128);           // 2 - 1073741824 Default: 128
  void SetTheshold(gint threshold = -60);     // -2147483648 - 0 Default: -60
  void SetPostMessage(gboolean pm = true);    // Default: true
  void SetMessagePhase(gboolean mp = false);  // Defaut: false
};

class ElementLevel : public ElementEx<ELEMENT_LEVEL> {
 public:
  typedef ElementEx<ELEMENT_LEVEL> base_class;
  using base_class::base_class;
};

}  // namespace audio
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
