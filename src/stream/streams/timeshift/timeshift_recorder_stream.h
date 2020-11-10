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

#include "stream/streams/timeshift/itimeshift_recorder_stream.h"

#include "utils/chunk_info.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {
class ElementSplitMuxSink;
}
}  // namespace elements
namespace streams {

namespace builders {
class TimeShiftRecorderStreamBuilder;
}

class TimeShiftRecorderStream : public ITimeShiftRecorderStream {
  friend class builders::TimeShiftRecorderStreamBuilder;

 public:
  typedef ITimeShiftRecorderStream base_class;
  TimeShiftRecorderStream(const TimeshiftConfig* config,
                          const TimeShiftInfo& info,
                          IStreamClient* client,
                          StreamStruct* stats);
  const char* ClassName() const override;
  ~TimeShiftRecorderStream() override;

 protected:
  virtual void OnSplitmuxsinkCreated(Connector conn, elements::sink::ElementSplitMuxSink* sink);
  chunk_index_t GetNextChunkStrategy(chunk_index_t last_index, time_t last_index_created_time) const override;

  IBaseBuilder* CreateBuilder() override;

  void HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) override;

  gboolean HandleMainTimerTick() override;
  void OnOutputDataFailed() override;
  virtual gchararray OnPathSet(GstElement* splitmux, guint fragment_id, GstSample* sample);

  chunk_index_t CalcNextIndex() const;
  utils::ChunkInfo chunk_;

 private:
  static gchararray path_setter_callback(GstElement* splitmux, guint fragment_id, gpointer user_data);
  static gchararray path_setter_full_callback(GstElement* splitmux,
                                              guint fragment_id,
                                              GstSample* sample,
                                              gpointer user_data);

  pad::Pad* audio_pad_;
  pad::Pad* video_pad_;
};

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
