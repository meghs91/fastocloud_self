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

#include "stream/elements/depay/audio.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace depay {

ElementRtpAACDePay* make_aac_depay(element_id_t pay_id) {
  ElementRtpAACDePay* aac_pay = make_audio_depay<ElementRtpAACDePay>(pay_id);
  return aac_pay;
}

ElementRtpAC3DePay* make_ac3_depay(element_id_t pay_id) {
  ElementRtpAC3DePay* aac_pay = make_audio_depay<ElementRtpAC3DePay>(pay_id);
  return aac_pay;
}

Element* make_audio_depay(const std::string& pay, const std::string& name) {
  if (pay == ElementRtpAACDePay::GetPluginName()) {
    return new ElementRtpAACDePay(name);
  } else if (pay == ElementRtpAC3DePay::GetPluginName()) {
    return new ElementRtpAC3DePay(name);
  }

  NOTREACHED();
  return nullptr;
}

}  // namespace depay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
