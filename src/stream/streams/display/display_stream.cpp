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

#include "stream/streams/display/display_stream.h"

#include "stream/streams/builders/display/display_input_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {

DisplayInputStream::DisplayInputStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : EncodingStream(config, client, stats) {}

const char* DisplayInputStream::ClassName() const {
  return "DisplayInputStream";
}

IBaseBuilder* DisplayInputStream::CreateBuilder() {
  const EncodeConfig* econf = static_cast<const EncodeConfig*>(GetConfig());
  return new builders::DisplayInputStreamBuilder(econf, this);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
