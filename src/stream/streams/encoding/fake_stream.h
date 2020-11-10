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

#include "stream/streams/encoding/encoding_stream.h"

namespace fastocloud {
namespace stream {
namespace streams {

class FakeStream : public EncodingStream {
 public:
  typedef EncodingStream base_class;
  FakeStream(EncodeConfig* config, IStreamClient* client);
  const char* ClassName() const override;
  ~FakeStream() override;

 protected:
  void PreLoop() override;
  void PostLoop(ExitStatus status) override;

  IBaseBuilder* CreateBuilder() override;

  gboolean HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message) override;

  void HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) override;
  void HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) override;

  gboolean HandleMainTimerTick() override;
};

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
