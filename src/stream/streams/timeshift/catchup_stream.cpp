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

#include "stream/streams/timeshift/catchup_stream.h"

#include <algorithm>

#include "base/constants.h"

#include "utils/m3u8_reader.h"
#include "utils/m3u8_writer.h"

#include "stream/gst_macros.h"

#define PLAYLIST_NAME "master.m3u8"

#include "stream/streams/builders/timeshift/catchup_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {

CatchupStream::CatchupStream(const TimeshiftConfig* config,
                             const TimeShiftInfo& info,
                             IStreamClient* client,
                             StreamStruct* stats)
    : base_class(config, info, client, stats), chunks_() {
  auto m3u8_path = info.timshift_dir.MakeFileStringPath(PLAYLIST_NAME);
  if (!m3u8_path) {
    return;
  }

  utils::M3u8Reader reader;
  reader.Parse(*m3u8_path);
  chunks_ = reader.GetChunks();
}

const char* CatchupStream::ClassName() const {
  return "CatchupStream";
}

chunk_index_t CatchupStream::GetNextChunkStrategy(chunk_index_t last_index, time_t last_index_created_time) const {
  UNUSED(last_index_created_time);
  return last_index + 1;
}

IBaseBuilder* CatchupStream::CreateBuilder() {
  const TimeshiftConfig* tconf = static_cast<const TimeshiftConfig*>(GetConfig());
  return new builders::CatchupStreamBuilder(tconf, this);
}

void CatchupStream::WriteM3u8List() {
  TimeShiftInfo tinf = GetTimeshiftInfo();
  auto m3u8_path = tinf.timshift_dir.MakeFileStringPath(PLAYLIST_NAME);
  if (!m3u8_path) {
    return;
  }

  utils::M3u8Writer fl;
  common::ErrnoError err =
      fl.Open(*m3u8_path, common::file_system::File::FLAG_CREATE | common::file_system::File::FLAG_WRITE);
  if (err) {
    return;
  }

  const TimeshiftConfig* tconf = static_cast<const TimeshiftConfig*>(GetConfig());
  time_t duration = tconf->GetTimeShiftChunkDuration();
  chunk_index_t first_index = 0;
  if (!chunks_.empty()) {
    first_index = chunks_[0].index;
  }
  err = fl.WriteHeader(first_index, duration);
  if (err) {
    WARNING_LOG() << "Failed to write m3u8 header to " << m3u8_path->GetPath() << ": " << err->GetDescription();
    return;
  }
  GstClockTime total_time = 0;
  for (size_t i = 0; i < chunks_.size(); ++i) {
    if (!GST_CLOCK_TIME_IS_VALID(chunks_[i].duration)) {
      chunks_[i].duration = duration * GST_SECOND;
    }
    total_time += chunks_[i].duration;
    err = fl.WriteLine(chunks_[i]);
    if (err) {
      WARNING_LOG() << "Failed to write chunk info to " << m3u8_path->GetPath() << ": " << err->GetDescription();
      return;
    }
  }
  err = fl.WriteFooter();
  if (err) {
    WARNING_LOG() << "Failed to write m3u8 footer to " << m3u8_path->GetPath() << ": " << err->GetDescription();
  } else {
    INFO_LOG() << "Catchup m3u8 file path: " << m3u8_path->GetPath() << " have been written successfully";
  }
}

void CatchupStream::PostLoop(ExitStatus status) {
  WriteM3u8List();
  base_class::PostLoop(status);
}

gchararray CatchupStream::OnPathSet(GstElement* splitmux, guint fragment_id, GstSample* sample) {
  const chunk_index_t ind = CalcNextIndex();
  const utils::ChunkInfo chunk(common::MemSPrintf("%llu." TS_EXTENSION, ind), GST_CLOCK_TIME_NONE, ind);

  if (sample) {
    GstBuffer* buffer = gst_sample_get_buffer(sample);
    if (buffer) {
      GstClockTime curr_time = GST_BUFFER_DTS_OR_PTS(buffer);
      if (GST_CLOCK_TIME_IS_VALID(curr_time) && GST_CLOCK_TIME_IS_VALID(chunk_.duration)) {
        GstClockTime diff = GST_CLOCK_DIFF(chunk_.duration, curr_time);
        if (!chunks_.empty()) {
          chunks_[chunks_.size() - 1].duration = diff;
        }
      }
      chunk_.duration = curr_time;
    }
  }

  chunks_.push_back(chunk);
  return base_class::OnPathSet(splitmux, fragment_id, sample);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
