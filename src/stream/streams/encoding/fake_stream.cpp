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

#include "stream/streams/encoding/fake_stream.h"

#include "stream/streams/builders/encoding/fake_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {

FakeStream::FakeStream(EncodeConfig* config, IStreamClient* client)
    : EncodingStream(config,
                     client,
                     new StreamStruct(StreamInfo{"fake",
                                                 fastotv::ENCODE,
                                                 {InputUri(0, common::uri::GURL())},
                                                 {OutputUri(0, common::uri::GURL())}})) {}

const char* FakeStream::ClassName() const {
  return "FakeStream";
}

FakeStream::~FakeStream() {
  StreamStruct* stat = GetStats();
  delete stat;
}

gboolean FakeStream::HandleMainTimerTick() {
  return TRUE;
}

void FakeStream::PreLoop() {}

void FakeStream::PostLoop(ExitStatus status) {
  UNUSED(status);
}

IBaseBuilder* FakeStream::CreateBuilder() {
  const EncodeConfig* econf = static_cast<const EncodeConfig*>(GetConfig());
  return new builders::FakeStreamBuilder(econf, this);
}

gboolean FakeStream::HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  GstMessageType type = GST_MESSAGE_TYPE(message);
  if (type == GST_MESSAGE_ERROR) {
    GError* err = nullptr;
    gchar* err_msg = nullptr;

    gst_message_parse_error(message, &err, &err_msg);
    if (err->code == G_FILE_ERROR_NOENT) {
      Quit(EXIT_INNER);
    }
    g_error_free(err);
    g_free(err_msg);
  }
  return EncodingStream::HandleAsyncBusMessageReceived(bus, message);
}

void FakeStream::HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  UNUSED(element);
}

void FakeStream::HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  UNUSED(element);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
