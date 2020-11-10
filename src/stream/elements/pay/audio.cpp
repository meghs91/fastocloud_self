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

#include "stream/elements/pay/audio.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace pay {

ElementRtpAACPay* make_aac_pay(guint pt, element_id_t pay_id) {
  ElementRtpAACPay* aac_pay = make_audio_pay<ElementRtpAACPay>(pay_id);
  aac_pay->SetPt(pt);
  return aac_pay;
}

ElementRtpAC3Pay* make_ac3_pay(guint pt, element_id_t pay_id) {
  ElementRtpAC3Pay* aac_pay = make_audio_pay<ElementRtpAC3Pay>(pay_id);
  aac_pay->SetPt(pt);
  return aac_pay;
}

Element* make_audio_pay(const std::string& pay, const std::string& name) {
  if (pay == ElementRtpAACPay::GetPluginName()) {
    return new ElementRtpAACPay(name);
  } else if (pay == ElementRtpAC3Pay::GetPluginName()) {
    return new ElementRtpAC3Pay(name);
  }

  NOTREACHED();
  return nullptr;
}

}  // namespace pay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
