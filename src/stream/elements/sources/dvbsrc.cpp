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

#include "stream/elements/sources/dvbsrc.h"

#include <vector>

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementDvbSrc::SetModulation(gint modulation) {
  SetProperty(MODULATION_PROPERTY, modulation);
}

void ElementDvbSrc::SetTransMode(gint trans) {
  SetProperty(TRANS_MODE_PROPERTY, trans);
}

void ElementDvbSrc::SetFrequency(guint frequency) {
  SetProperty(FREQUENCY_PROPERTY, frequency);
}

void ElementDvbSrc::SetPolarity(const std::string& polarity) {
  SetProperty(POLARITY_PROPERTY, polarity);
}

ElementDvbSrc* make_dvb_src(element_id_t input_id) {
  return make_sources<ElementDvbSrc>(input_id);
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
