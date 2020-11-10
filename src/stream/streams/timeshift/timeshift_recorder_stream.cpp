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

#include "stream/streams/timeshift/timeshift_recorder_stream.h"

#include <string>

#include <common/file_system/string_path_utils.h>
#include <common/time.h>

#include "base/constants.h"
#include "base/utils.h"

#include "stream/elements/sink/sink.h"
#include "stream/pad/pad.h"
#include "stream/streams/builders/timeshift/timeshift_recorder_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace {
pad::Pad* link_to_multiplexer(elements::Element* src, elements::Element* sink, const char* pad_name) {
  pad::Pad* src_pad = src->StaticPad("src");
  if (!src_pad->IsValid()) {
    delete src_pad;
    return nullptr;
  }
  pad::Pad* sink_pad = sink->RequestPad(pad_name);
  if (!sink_pad->IsValid()) {
    delete src_pad;
    delete sink_pad;
    return nullptr;
  }

  GstPadLinkReturn ret = gst_pad_link(src_pad->GetGstPad(), sink_pad->GetGstPad());
  DCHECK(GST_PAD_LINK_SUCCESSFUL(ret));
  delete src_pad;
  return sink_pad;
}
}  // namespace
namespace streams {

TimeShiftRecorderStream::TimeShiftRecorderStream(const TimeshiftConfig* config,
                                                 const TimeShiftInfo& info,
                                                 IStreamClient* client,
                                                 StreamStruct* stats)
    : base_class(config, info, client, stats), chunk_(), audio_pad_(nullptr), video_pad_(nullptr) {}

const char* TimeShiftRecorderStream::ClassName() const {
  return "TimeShiftRecorderStream";
}

TimeShiftRecorderStream::~TimeShiftRecorderStream() {
  elements::Element* splitmuxsink = GetElementByName(common::MemSPrintf(SPLIT_SINK_NAME_1U, 0));
  if (audio_pad_) {
    splitmuxsink->ReleaseRequestedPad(audio_pad_);
  }
  if (video_pad_) {
    splitmuxsink->ReleaseRequestedPad(video_pad_);
  }
  destroy(&audio_pad_);
  destroy(&video_pad_);
}

void TimeShiftRecorderStream::OnSplitmuxsinkCreated(Connector conn, elements::sink::ElementSplitMuxSink* sink) {
  TimeShiftInfo tinfo = GetTimeshiftInfo();
  chunk_index_t index = invalid_chunk_index;
  time_t file_created_time = 0;
  if (tinfo.FindLastChunk(&index, &file_created_time)) {
    index = GetNextChunkStrategy(index, file_created_time);
  }
  chunk_ = {tinfo.timshift_dir.GetPath(), index, GST_CLOCK_TIME_NONE};
#if GST_CHECK_VERSION(1, 11, 1)
  gboolean res = sink->RegisterFormatLocationFullCallback(TimeShiftRecorderStream::path_setter_full_callback, this);
  DCHECK(res);
#endif

  audio_pad_ = link_to_multiplexer(conn.audio, sink, "audio_%u");
  video_pad_ = link_to_multiplexer(conn.video, sink, "video");
}

chunk_index_t TimeShiftRecorderStream::GetNextChunkStrategy(chunk_index_t last_index,
                                                            time_t last_index_created_time) const {
  const TimeshiftConfig* tconf = static_cast<const TimeshiftConfig*>(GetConfig());
  time_t cur_time = common::time::current_utc_mstime() / 1000;  // OK
  time_t diff = cur_time - last_index_created_time;
  if (diff > 0) {
    last_index += diff / tconf->GetTimeShiftChunkDuration();
  }

  return last_index;
}

IBaseBuilder* TimeShiftRecorderStream::CreateBuilder() {
  const TimeshiftConfig* tconf = static_cast<const TimeshiftConfig*>(GetConfig());
  return new builders::TimeShiftRecorderStreamBuilder(tconf, this);
}

void TimeShiftRecorderStream::HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  DEBUG_LOG() << "decodebin added element: " << element_plugin_name;
}

gboolean TimeShiftRecorderStream::HandleMainTimerTick() {
  TimeShiftInfo tinfo = GetTimeshiftInfo();
  time_t el = GetElipsedTime();
  if (el % no_data_panic_sec == 0) {
    const time_t max_life_time = common::time::current_utc_mstime() / 1000 - tinfo.timeshift_chunk_life_time;
    RemoveOldFilesByTime(tinfo.timshift_dir, max_life_time, "*" CHUNK_EXT);
  }
  return base_class::HandleMainTimerTick();
}

void TimeShiftRecorderStream::OnOutputDataFailed() {
  OnOutputDataOK();
}

chunk_index_t TimeShiftRecorderStream::CalcNextIndex() const {
  chunk_index_t index = chunk_.index;
  if (index == invalid_chunk_index) {
    index = 0;
  }

  std::string old_path = common::MemSPrintf("%s%llu." TS_EXTENSION, chunk_.path, index);
  if (common::file_system::is_file_exist(old_path)) {  // if chunk exist move to next
    index++;
  }

  return index;
}

gchararray TimeShiftRecorderStream::OnPathSet(GstElement* splitmux, guint fragment_id, GstSample* sample) {
  UNUSED(splitmux);
  UNUSED(fragment_id);
  UNUSED(sample);

  chunk_index_t ind = CalcNextIndex();
  chunk_.index = ind;
  std::string new_path = common::MemSPrintf("%s%llu." TS_EXTENSION, chunk_.path, chunk_.index);
  return strdup(new_path.c_str());
}

gchararray TimeShiftRecorderStream::path_setter_callback(GstElement* splitmux, guint fragment_id, gpointer user_data) {
  return path_setter_full_callback(splitmux, fragment_id, nullptr, user_data);
}

gchararray TimeShiftRecorderStream::path_setter_full_callback(GstElement* splitmux,
                                                              guint fragment_id,
                                                              GstSample* sample,
                                                              gpointer user_data) {
  UNUSED(splitmux);

  TimeShiftRecorderStream* stream = reinterpret_cast<TimeShiftRecorderStream*>(user_data);
  return stream->OnPathSet(splitmux, fragment_id, sample);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
