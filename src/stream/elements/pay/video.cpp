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

#include "stream/elements/pay/video.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace pay {

ElementRtpMPEG2Pay* make_mpeg2_pay(guint pt, element_id_t pay_id) {
  ElementRtpMPEG2Pay* h264_pay = make_video_pay<ElementRtpMPEG2Pay>(pay_id);
  h264_pay->SetPt(pt);
  return h264_pay;
}

ElementRtpH264Pay* make_h264_pay(guint pt, element_id_t pay_id) {
  ElementRtpH264Pay* h264_pay = make_video_pay<ElementRtpH264Pay>(pay_id);
  h264_pay->SetPt(pt);
  return h264_pay;
}

ElementRtpH265Pay* make_h265_pay(guint pt, element_id_t pay_id) {
  ElementRtpH265Pay* h265_pay = make_video_pay<ElementRtpH265Pay>(pay_id);
  h265_pay->SetPt(pt);
  return h265_pay;
}

Element* make_video_pay(const std::string& pay, const std::string& name) {
  if (pay == ElementRtpH264Pay::GetPluginName()) {
    return new ElementRtpH264Pay(name);
  } else if (pay == ElementRtpH265Pay::GetPluginName()) {
    return new ElementRtpH265Pay(name);
  } else if (pay == ElementRtpMPEG2Pay::GetPluginName()) {
    return new ElementRtpMPEG2Pay(name);
  }

  NOTREACHED();
  return nullptr;
}

}  // namespace pay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
