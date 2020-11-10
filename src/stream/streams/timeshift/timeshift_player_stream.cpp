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

#include "stream/streams/timeshift/timeshift_player_stream.h"

#include "stream/streams/builders/timeshift/timeshift_player_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {

TimeShiftPlayerStream::TimeShiftPlayerStream(const RelayConfig* config,
                                             const TimeShiftInfo& info,
                                             IStreamClient* client,
                                             StreamStruct* stats,
                                             chunk_index_t start_chunk_index)
    : base_class(config, client, stats), timeshift_info_(info), start_chunk_index_(start_chunk_index) {}

const char* TimeShiftPlayerStream::ClassName() const {
  return "TimeShiftPlayerStream";
}

TimeShiftInfo TimeShiftPlayerStream::GetTimeshiftInfo() const {
  return timeshift_info_;
}

IBaseBuilder* TimeShiftPlayerStream::CreateBuilder() {
  const RelayConfig* rconf = static_cast<const RelayConfig*>(GetConfig());
  return new builders::TimeShiftPlayerBuilder(GetTimeshiftInfo(), start_chunk_index_, rconf, this);
}

void TimeShiftPlayerStream::OnInputDataFailed() {
  OnInputDataOK();
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
