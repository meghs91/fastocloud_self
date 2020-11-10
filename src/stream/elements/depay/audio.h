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

#include "stream/stypes.h"

#include "stream/elements/depay/depay.h"  // for ElementRtpPay
#include "stream/elements/element.h"      // for Element (ptr only), SupportedElements:...

namespace fastocloud {
namespace stream {
namespace elements {
namespace depay {

class ElementRtpAACDePay : public ElementRtpDePay<ELEMENT_RTP_AAC_DEPAY> {
 public:
  typedef ElementRtpDePay<ELEMENT_RTP_AAC_DEPAY> base_class;
  using base_class::base_class;
};

class ElementRtpAC3DePay : public ElementRtpDePay<ELEMENT_RTP_AC3_DEPAY> {
 public:
  typedef ElementRtpDePay<ELEMENT_RTP_AC3_DEPAY> base_class;
  using base_class::base_class;
};

template <typename T>
T* make_audio_depay(element_id_t pay_id) {
  return make_element<T>(common::MemSPrintf(AUDIO_DEPAY_NAME_1U, pay_id));
}

ElementRtpAACDePay* make_aac_depay(element_id_t pay_id);
ElementRtpAC3DePay* make_ac3_depay(element_id_t pay_id);

Element* make_audio_depay(const std::string& pay, const std::string& name);

}  // namespace depay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
