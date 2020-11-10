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

#include "stream/streams/builders/timeshift/timeshift_recorder_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

class CatchupStreamBuilder : public TimeShiftRecorderStreamBuilder {
 public:
  typedef TimeShiftRecorderStreamBuilder base_class;
  CatchupStreamBuilder(const TimeshiftConfig* api, TimeShiftRecorderStream* observer);
};

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
