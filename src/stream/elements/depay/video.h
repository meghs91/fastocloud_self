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

// for element_id_t, VIDEO_PAY_NAME_1U

#include "stream/elements/depay/depay.h"  // for ElementRtpPay
#include "stream/elements/element.h"      // for Element (ptr only), SupportedElements:...

#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace depay {

class ElementRtpMPEG2DePay : public ElementRtpDePay<ELEMENT_RTP_MPEG2_DEPAY> {
 public:
  typedef ElementRtpDePay<ELEMENT_RTP_MPEG2_DEPAY> base_class;
  using base_class::base_class;
};

class ElementRtpH264DePay : public ElementRtpDePay<ELEMENT_RTP_H264_DEPAY> {
 public:
  typedef ElementRtpDePay<ELEMENT_RTP_H264_DEPAY> base_class;
  using base_class::base_class;
};

class ElementRtpH265DePay : public ElementRtpDePay<ELEMENT_RTP_H265_DEPAY> {
 public:
  typedef ElementRtpDePay<ELEMENT_RTP_H265_DEPAY> base_class;
  using base_class::base_class;
};

template <typename T>
T* make_video_depay(element_id_t pay_id) {
  return make_element<T>(common::MemSPrintf(VIDEO_DEPAY_NAME_1U, pay_id));
}

ElementRtpMPEG2DePay* make_mpeg2_depay(element_id_t pay_id);
ElementRtpH264DePay* make_h264_depay(element_id_t pay_id);
ElementRtpH265DePay* make_h265_depay(element_id_t pay_id);

Element* make_video_depay(const std::string& pay, const std::string& name);

}  // namespace depay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
