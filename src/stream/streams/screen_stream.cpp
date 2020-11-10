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

#include "stream/streams/screen_stream.h"

#include "stream/streams/builders/screen_stream_builder.h"

#include "stream/pad/pad.h"

namespace fastocloud {
namespace stream {
namespace streams {

ScreenStream::ScreenStream(AudioVideoConfig* config, IStreamClient* client, StreamStruct* stats)
    : IBaseStream(config, client, stats) {}

const char* ScreenStream::ClassName() const {
  return "ScreenStream";
}

void ScreenStream::OnInpudSrcPadCreated(pad::Pad* src_pad, element_id_t id, const common::uri::GURL& url) {
  UNUSED(src_pad);
  UNUSED(id);
  UNUSED(url);
  // LinkInputPad(sink_pad);
}

void ScreenStream::OnOutputSinkPadCreated(pad::Pad* sink_pad,
                                          element_id_t id,
                                          const common::uri::GURL& url,
                                          bool need_push) {
  LinkOutputPad(sink_pad->GetGstPad(), id, url, need_push);
}

IBaseBuilder* ScreenStream::CreateBuilder() {
  const AudioVideoConfig* aconf = static_cast<const AudioVideoConfig*>(GetConfig());
  return new builders::ScreenStreamBuilder(aconf, this);
}

void ScreenStream::PreLoop() {}

void ScreenStream::PostLoop(ExitStatus status) {
  UNUSED(status);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
