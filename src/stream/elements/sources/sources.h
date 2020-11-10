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

#include <string>

#include <common/sprintf.h>

#include "stream/elements/element.h"  // for ElementEx, SupportedElements, Supporte...

#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

template <SupportedElements el>
class ElementBaseSrc : public ElementEx<el> {
 public:
  typedef ElementEx<el> base_class;
  using base_class::base_class;
};

template <SupportedElements el>
class ElementPushSrc : public ElementBaseSrc<el> {
 public:
  typedef ElementBaseSrc<el> base_class;
  using base_class::base_class;
};

typedef ElementPushSrc<ELEMENT_VIDEO_TEST_SRC> ElementVideoTestSrc;
typedef ElementPushSrc<ELEMENT_AUDIO_TEST_SRC> ElementAudioTestSrc;
typedef ElementPushSrc<ELEMENT_DISPLAY_SRC> ElementDisplayTestSrc;

template <SupportedElements el>
class ElementLocation : public ElementPushSrc<el> {
 public:
  typedef ElementPushSrc<el> base_class;
  using base_class::base_class;

  void SetLocation(const std::string& location) { base_class::SetProperty("location", location); }
};

template <typename T>
T* make_sources(element_id_t source_id) {
  return make_element<T>(common::MemSPrintf(SRC_NAME_1U, source_id));
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
