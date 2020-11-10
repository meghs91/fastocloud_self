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

#include <common/sprintf.h>

#include "stream/elements/element.h"
#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace muxer {

class ElementMPEGTSMux : public ElementEx<ELEMENT_MPEGTS_MUX> {
 public:
  typedef ElementEx<ELEMENT_MPEGTS_MUX> base_class;
  using base_class::base_class;
};

typedef ElementEx<ELEMENT_RTP_MUX> ElementRTPMux;

class ElementFLVMux : public ElementEx<ELEMENT_FLV_MUX> {
 public:
  typedef ElementEx<ELEMENT_FLV_MUX> base_class;
  using base_class::base_class;

  void SetStreamable(bool streamable = false);  // Default: false
};

template <typename T>
T* make_muxer(element_id_t muxer_id) {
  return make_element<T>(common::MemSPrintf(MUXER_NAME_1U, muxer_id));
}

ElementFLVMux* make_flvmux(bool streamable, element_id_t muxer_id);
ElementRTPMux* make_rtpmux(element_id_t muxer_id);
ElementMPEGTSMux* make_mpegtsmux(element_id_t muxer_id);

Element* make_muxer(const common::uri::GURL& url, element_id_t muxer_id);

}  // namespace muxer
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
