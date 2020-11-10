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

#include "stream/streams/builders/timeshift/timeshift_recorder_stream_builder.h"

#include <common/sprintf.h>

#include "stream/elements/muxer/muxer.h"
#include "stream/elements/sink/sink.h"

#include "stream/streams/timeshift/timeshift_recorder_stream.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

TimeShiftRecorderStreamBuilder::TimeShiftRecorderStreamBuilder(const TimeshiftConfig* api,
                                                               TimeShiftRecorderStream* observer)
    : base_class(api, observer) {}

Connector TimeShiftRecorderStreamBuilder::BuildOutput(Connector conn) {
  elements::sink::ElementSplitMuxSink* splitmuxsink =
      new elements::sink::ElementSplitMuxSink(common::MemSPrintf(SPLIT_SINK_NAME_1U, 0));
  ElementAdd(splitmuxsink);

  const TimeshiftConfig* tconf = static_cast<const TimeshiftConfig*>(GetConfig());
  elements::muxer::ElementMPEGTSMux* mpegtsmux = elements::muxer::make_mpegtsmux(0);
  guint64 mst_nsec = tconf->GetTimeShiftChunkDuration() * GST_SECOND;
  splitmuxsink->SetMuxer(mpegtsmux);
  delete mpegtsmux;

  splitmuxsink->SetMaxSizeTime(mst_nsec);
  HandleSplitmuxsinkCreated(conn, splitmuxsink);
  return conn;
}

Connector TimeShiftRecorderStreamBuilder::BuildConverter(Connector conn) {
  return conn;
}

void TimeShiftRecorderStreamBuilder::HandleSplitmuxsinkCreated(Connector conn,
                                                               elements::sink::ElementSplitMuxSink* sink) {
  TimeShiftRecorderStream* stream = static_cast<TimeShiftRecorderStream*>(GetObserver());
  if (stream) {
    stream->OnSplitmuxsinkCreated(conn, sink);
  }
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
