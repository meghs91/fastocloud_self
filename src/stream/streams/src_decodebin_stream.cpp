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

#include "stream/streams/src_decodebin_stream.h"

#include <string>

#include "stream/config.h"
#include "stream/pad/pad.h"

namespace fastocloud {
namespace stream {
namespace streams {

void SrcDecodeBinStream::ConnectDecodebinSignals(elements::ElementDecodebin* decodebin) {
  gboolean pad_added = decodebin->RegisterPadAddedCallback(decodebin_pad_added_callback, this);
  DCHECK(pad_added);

  gboolean autoplug_continue = decodebin->RegisterAutoplugContinue(decodebin_autoplugger_callback, this);
  DCHECK(autoplug_continue);

  gboolean autoplug_select = decodebin->RegisterAutoplugSelect(decodebin_autoplug_select_callback, this);
  DCHECK(autoplug_select);

  gboolean autoplug_sort = decodebin->RegisterAutoplugSort(decodebin_autoplug_sort_callback, this);
  DCHECK(autoplug_sort);

  gboolean element_added = decodebin->RegisterElementAdded(decodebin_element_added_callback, this);
  DCHECK(element_added);

  gboolean element_removed = decodebin->RegisterElementRemoved(decodebin_element_removed_callback, this);
  DCHECK(element_removed);
}

SrcDecodeBinStream::SrcDecodeBinStream(const Config* config, IStreamClient* client, StreamStruct* stats)
    : IBaseStream(config, client, stats) {}

const char* SrcDecodeBinStream::ClassName() const {
  return "SrcDecodeBinStream";
}

void SrcDecodeBinStream::PreLoop() {
  const Config* conf = GetConfig();
  const auto input = conf->GetUrl();
  if (client_) {
    client_->OnInputChanged(this, input[0]);
  }
}

void SrcDecodeBinStream::PostLoop(ExitStatus status) {
  UNUSED(status);
}

void SrcDecodeBinStream::decodebin_pad_added_callback(GstElement* src, GstPad* new_pad, gpointer user_data) {
  SrcDecodeBinStream* stream = reinterpret_cast<SrcDecodeBinStream*>(user_data);
  stream->HandleDecodeBinPadAdded(src, new_pad);
}

gboolean SrcDecodeBinStream::decodebin_autoplugger_callback(GstElement* elem,
                                                            GstPad* pad,
                                                            GstCaps* caps,
                                                            gpointer user_data) {
  SrcDecodeBinStream* stream = reinterpret_cast<SrcDecodeBinStream*>(user_data);
  return stream->HandleDecodeBinAutoplugger(elem, pad, caps);
}

GstAutoplugSelectResult SrcDecodeBinStream::decodebin_autoplug_select_callback(GstElement* bin,
                                                                               GstPad* pad,
                                                                               GstCaps* caps,
                                                                               GstElementFactory* factory,
                                                                               gpointer user_data) {
  SrcDecodeBinStream* stream = reinterpret_cast<SrcDecodeBinStream*>(user_data);
  return stream->HandleAutoplugSelect(bin, pad, caps, factory);
}

GValueArray* SrcDecodeBinStream::decodebin_autoplug_sort_callback(GstElement* bin,
                                                                  GstPad* pad,
                                                                  GstCaps* caps,
                                                                  GValueArray* factories,
                                                                  gpointer user_data) {
  SrcDecodeBinStream* stream = reinterpret_cast<SrcDecodeBinStream*>(user_data);
  return stream->HandleAutoplugSort(bin, pad, caps, factories);
}

void SrcDecodeBinStream::decodebin_element_added_callback(GstBin* bin, GstElement* element, gpointer user_data) {
  SrcDecodeBinStream* stream = reinterpret_cast<SrcDecodeBinStream*>(user_data);
  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  if (element_plugin_name == elements::ElementTsDemux::GetPluginName()) {
    const auto config = stream->GetConfig();
    input_t input = config->GetUrl();
    for (size_t i = 0; i < input.size(); ++i) {
      common::uri::GURL input_url = input[i].GetUrl();
      if (input_url.SchemeIsUdp()) {
        const auto pid = input[i].GetProgramNumber();
        if (pid) {
          elements::ElementTsDemux* tsdemux = new elements::ElementTsDemux("demux", element);
          tsdemux->SetProgramNumber(*pid);
          delete tsdemux;
        }
      }
    }
  }

  return stream->HandleDecodeBinElementAdded(bin, element);
}

void SrcDecodeBinStream::decodebin_element_removed_callback(GstBin* bin, GstElement* element, gpointer user_data) {
  SrcDecodeBinStream* stream = reinterpret_cast<SrcDecodeBinStream*>(user_data);
  return stream->HandleDecodeBinElementRemoved(bin, element);
}

void SrcDecodeBinStream::OnInpudSrcPadCreated(pad::Pad* src_pad, element_id_t id, const common::uri::GURL& url) {
  LinkInputPad(src_pad->GetGstPad(), id, url);
}

void SrcDecodeBinStream::OnOutputSinkPadCreated(pad::Pad* sink_pad,
                                                element_id_t id,
                                                const common::uri::GURL& url,
                                                bool need_push) {
  LinkOutputPad(sink_pad->GetGstPad(), id, url, need_push);
}

void SrcDecodeBinStream::OnDecodebinCreated(elements::ElementDecodebin* decodebin) {
  ConnectDecodebinSignals(decodebin);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
