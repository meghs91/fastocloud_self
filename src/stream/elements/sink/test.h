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

#include "stream/elements/element.h"    // for SupportedElements::ELEMENT_FAKE_SINK
#include "stream/elements/sink/sink.h"  // for ElementBaseSink
#include "stream/stypes.h"

// for element_id_

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

class ElementTestSink : public ElementBaseSink<ELEMENT_TEST_SINK> {
 public:
  typedef ElementBaseSink<ELEMENT_TEST_SINK> base_class;
  using base_class::base_class;
};

ElementTestSink* make_test_sink(element_id_t sink_id);

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
