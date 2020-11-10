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

#include "stream/elements/element.h"  // for Element (ptr only), SupportedElements:...
#include "stream/elements/pay/pay.h"  // for ElementRtpPay

#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace pay {

class ElementRtpMPEG2Pay : public ElementRtpPay<ELEMENT_RTP_MPEG2_PAY> {
 public:
  typedef ElementRtpPay<ELEMENT_RTP_MPEG2_PAY> base_class;
  using base_class::base_class;
};

class ElementRtpH264Pay : public ElementRtpPay<ELEMENT_RTP_H264_PAY> {
 public:
  typedef ElementRtpPay<ELEMENT_RTP_H264_PAY> base_class;
  using base_class::base_class;
};

class ElementRtpH265Pay : public ElementRtpPay<ELEMENT_RTP_H265_PAY> {
 public:
  typedef ElementRtpPay<ELEMENT_RTP_H265_PAY> base_class;
  using base_class::base_class;
};

template <typename T>
T* make_video_pay(element_id_t pay_id) {
  return make_element<T>(common::MemSPrintf(VIDEO_PAY_NAME_1U, pay_id));
}

ElementRtpMPEG2Pay* make_mpeg2_pay(guint pt, element_id_t pay_id);
ElementRtpH264Pay* make_h264_pay(guint pt, element_id_t pay_id);
ElementRtpH265Pay* make_h265_pay(guint pt, element_id_t pay_id);

Element* make_video_pay(const std::string& pay, const std::string& name);

}  // namespace pay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
