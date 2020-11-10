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

#include "stream/elements/depay/video.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace depay {

ElementRtpMPEG2DePay* make_mpeg2_depay(element_id_t pay_id) {
  ElementRtpMPEG2DePay* h264_pay = make_video_depay<ElementRtpMPEG2DePay>(pay_id);
  return h264_pay;
}

ElementRtpH264DePay* make_h264_depay(element_id_t pay_id) {
  ElementRtpH264DePay* h264_pay = make_video_depay<ElementRtpH264DePay>(pay_id);
  return h264_pay;
}

ElementRtpH265DePay* make_h265_depay(element_id_t pay_id) {
  ElementRtpH265DePay* h265_pay = make_video_depay<ElementRtpH265DePay>(pay_id);
  return h265_pay;
}

Element* make_video_depay(const std::string& pay, const std::string& name) {
  if (pay == ElementRtpH264DePay::GetPluginName()) {
    return new ElementRtpH264DePay(name);
  } else if (pay == ElementRtpH265DePay::GetPluginName()) {
    return new ElementRtpH265DePay(name);
  } else if (pay == ElementRtpMPEG2DePay::GetPluginName()) {
    return new ElementRtpMPEG2DePay(name);
  }

  NOTREACHED();
  return nullptr;
}

}  // namespace depay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
