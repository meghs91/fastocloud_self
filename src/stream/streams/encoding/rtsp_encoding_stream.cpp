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

#include "stream/streams/encoding/rtsp_encoding_stream.h"

#include <string>

#include "stream/streams/builders/encoding/rtsp_stream_builder.h"

#include "stream/elements/sources/rtspsrc.h"

#include "stream/gstreamer_utils.h"  // for pad_get_type
#include "stream/pad/pad.h"          // for Pad

namespace fastocloud {
namespace stream {
namespace streams {

RtspEncodingStream::RtspEncodingStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : EncodingStream(config, client, stats) {}

const char* RtspEncodingStream::ClassName() const {
  return "RtspEncodingStream";
}

IBaseBuilder* RtspEncodingStream::CreateBuilder() {
  const EncodeConfig* econf = static_cast<const EncodeConfig*>(GetConfig());
  return new builders::RtspEncodingStreamBuilder(econf, this);
}

void RtspEncodingStream::OnRTSPSrcCreated(elements::sources::ElementRTSPSrc* src) {
  gboolean pad_added = src->RegisterPadAddedCallback(rtspsrc_pad_added_callback, this);
  DCHECK(pad_added);
}

void RtspEncodingStream::rtspsrc_pad_added_callback(GstElement* src, GstPad* new_pad, gpointer user_data) {
  RtspEncodingStream* stream = reinterpret_cast<RtspEncodingStream*>(user_data);
  stream->HandleRtspSrcPadAdded(src, new_pad);
}

void RtspEncodingStream::HandleRtspSrcPadAdded(GstElement* src, GstPad* new_pad) {
  const gchar* new_pad_type = pad_get_type(new_pad);
  if (!new_pad_type) {
    DNOTREACHED();
    return;
  }

  INFO_LOG() << "RTP Pad added: " << new_pad_type;
  elements::Element* dest = GetElementByName(common::MemSPrintf(DECODEBIN_NAME_1U, 0));

  if (!dest) {
    return;
  }

  pad::Pad* sink_pad = dest->StaticPad("sink");
  if (!sink_pad->IsValid()) {
    return;
  }

  if (!gst_pad_is_linked(sink_pad->GetGstPad())) {
    GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad->GetGstPad());
    if (GST_PAD_LINK_FAILED(ret)) {
    } else {
      DEBUG_LOG() << "Pad emitted: " << GST_ELEMENT_NAME(src) << " " << GST_PAD_NAME(new_pad) << " " << new_pad_type;
    }
  } else {
    DEBUG_LOG() << "pad-emitter: pad is linked";
  }

  const std::string element_plugin_name = elements::Element::GetPluginName(src);
  if (element_plugin_name == elements::sources::ElementRTSPSrc::GetPluginName()) {
    elements::sources::ElementRTSPSrc* rsrc = new elements::sources::ElementRTSPSrc("src", src);
    pad::Pad* src_pad = new pad::Pad(new_pad);
    OnInpudSrcPadCreated(src_pad, 0, common::uri::GURL(rsrc->GetLocation()));
    delete src_pad;
    delete rsrc;
  }
  delete sink_pad;
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
