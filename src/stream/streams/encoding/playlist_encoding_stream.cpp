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

#include "stream/streams/encoding/playlist_encoding_stream.h"

#include <string>

#include <gst/app/gstappsrc.h>  // for GST_APP_SRC

#include "stream/elements/sources/appsrc.h"

#include "stream/streams/builders/encoding/playlist_encoding_stream_builder.h"

#define BUFFER_SIZE 4096

namespace fastocloud {
namespace stream {
namespace streams {

PlaylistEncodingStream::PlaylistEncodingStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : EncodingStream(config, client, stats), app_src_(nullptr), current_file_(nullptr), curent_pos_(0) {}

PlaylistEncodingStream::~PlaylistEncodingStream() {
  if (current_file_) {
    fclose(current_file_);
    current_file_ = nullptr;
  }
}

const char* PlaylistEncodingStream::ClassName() const {
  return "PlaylistEncodingStream";
}

void PlaylistEncodingStream::OnAppSrcCreatedCreated(elements::sources::ElementAppSrc* src) {
  app_src_ = src;
  gboolean res = src->RegisterNeedDataCallback(PlaylistEncodingStream::need_data_callback, this);
  DCHECK(res);
}

IBaseBuilder* PlaylistEncodingStream::CreateBuilder() {
  const PlaylistEncodeConfig* econf = static_cast<const PlaylistEncodeConfig*>(GetConfig());
  return new builders::PlaylistEncodingStreamBuilder(econf, this);
}

void PlaylistEncodingStream::PreLoop() {}

void PlaylistEncodingStream::HandleNeedData(GstElement* pipeline, guint rsize) {
  UNUSED(pipeline);
  UNUSED(rsize);

  size_t size = 0;
  char* ptr = static_cast<char*>(calloc(BUFFER_SIZE, sizeof(char)));
  if (!ptr) {
    app_src_->SendEOS();
    return;
  }

  while (size == 0) {
    if (!current_file_) {
      current_file_ = OpenNextFile();
    }

    if (!current_file_) {
      app_src_->SendEOS();
      free(ptr);
      return;
    }

    size = fread(ptr, sizeof(char), BUFFER_SIZE, current_file_);
    if (size == 0) {
      fclose(current_file_);
      current_file_ = nullptr;
    }
  }

  GstBuffer* buffer = gst_buffer_new_wrapped(ptr, size);
  GstFlowReturn ret = app_src_->PushBuffer(buffer);
  if (ret != GST_FLOW_OK) {
    WARNING_LOG() << "gst_app_src_push_buffer failed: " << gst_flow_get_name(ret);
    Quit(EXIT_INNER);
  }
}

void PlaylistEncodingStream::need_data_callback(GstElement* pipeline, guint size, gpointer user_data) {
  PlaylistEncodingStream* stream = reinterpret_cast<PlaylistEncodingStream*>(user_data);
  return stream->HandleNeedData(pipeline, size);
}

FILE* PlaylistEncodingStream::OpenNextFile() {
  const PlaylistEncodeConfig* econf = static_cast<const PlaylistEncodeConfig*>(GetConfig());
  const auto loop = econf->GetLoop();

  if (!loop) {
    input_t input = econf->GetUrl();
    if (curent_pos_ >= input.size()) {
      INFO_LOG() << "No more files for playing";
      return nullptr;  // EOS
    }
  }

  input_t input = econf->GetUrl();
  if (curent_pos_ >= input.size()) {
    curent_pos_ = 0;
  }

  InputUri iuri = input[curent_pos_];
  common::uri::GURL uri = iuri.GetUrl();
  curent_pos_++;
  std::string cur_path = uri.path();
  FILE* file = fopen(cur_path.c_str(), "rb");
  if (file) {
    INFO_LOG() << "File " << cur_path << " open for playing";
    if (client_) {
      client_->OnInputChanged(this, iuri);
    }
  } else {
    WARNING_LOG() << "File " << cur_path << " can't open for playing";
  }
  return file;
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
