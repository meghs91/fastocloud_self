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

#include "stream/streams/builders/relay/relay_stream_builder.h"

#define SPLIT_SINK_NAME_1U "splitmuxsink_%lu"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {
class ElementSplitMuxSink;
}
}  // namespace elements
namespace streams {
class TimeShiftRecorderStream;
namespace builders {

class TimeShiftRecorderStreamBuilder : public RelayStreamBuilder {
 public:
  typedef RelayStreamBuilder base_class;
  TimeShiftRecorderStreamBuilder(const TimeshiftConfig* api, TimeShiftRecorderStream* observer);

  Connector BuildOutput(Connector conn) override;
  Connector BuildConverter(Connector conn) override;

 protected:
  void HandleSplitmuxsinkCreated(Connector conn, elements::sink::ElementSplitMuxSink* sink);
};

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
