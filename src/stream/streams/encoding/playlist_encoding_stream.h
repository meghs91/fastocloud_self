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

namespace elements {
namespace sources {
class ElementAppSrc;
}
}  // namespace elements

namespace streams {

namespace builders {
class PlaylistEncodingStreamBuilder;
}

class PlaylistEncodingStream : public EncodingStream {
  friend class builders::PlaylistEncodingStreamBuilder;

 public:
  PlaylistEncodingStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats);
  ~PlaylistEncodingStream() override;

  const char* ClassName() const override;

 protected:
  void PreLoop() override;

  virtual void OnAppSrcCreatedCreated(elements::sources::ElementAppSrc* src);
  IBaseBuilder* CreateBuilder() override;

  virtual void HandleNeedData(GstElement* pipeline, guint rsize);

 private:
  static void need_data_callback(GstElement* pipeline, guint size, gpointer user_data);

  FILE* OpenNextFile();

  elements::sources::ElementAppSrc* app_src_;
  FILE* current_file_;
  size_t curent_pos_;
};

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
