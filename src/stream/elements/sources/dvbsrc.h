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
#include <vector>

#include "stream/elements/sources/sources.h"  // for ElementLocation

#define MODULATION_PROPERTY "modulation"
#define TRANS_MODE_PROPERTY "trans-mode"
#define FREQUENCY_PROPERTY "frequency"
#define POLARITY_PROPERTY "polarity"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

class ElementDvbSrc : public ElementPushSrc<ELEMENT_DVB_SRC> {
 public:
  typedef ElementPushSrc<ELEMENT_DVB_SRC> base_class;
  using base_class::base_class;
  static const gint MODULATION_DEFAULT = 1;
  static const gint TRANS_MODE_DEFAULT = 1;

  void SetModulation(gint modulation = MODULATION_DEFAULT);  // 0 - 13 Default: 1
  void SetTransMode(gint trans = TRANS_MODE_DEFAULT);        // 0 - 8 Default: 1
  void SetFrequency(guint frequency = 0);                    // 0 - 4294967295 Default: 0
  void SetPolarity(const std::string& polarity = "h");       // [vhHV] (eg. V for Vertical
};

ElementDvbSrc* make_dvb_src(element_id_t input_id);

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
