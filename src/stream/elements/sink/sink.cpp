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

#include "stream/elements/sink/sink.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

gboolean ElementSplitMuxSink::RegisterFormatLocationCallback(format_location_callback_t cb, gpointer user_data) {
  return RegisterCallback("format-location", G_CALLBACK(cb), user_data);
}

#if GST_CHECK_VERSION(1, 11, 1)
gboolean ElementSplitMuxSink::RegisterFormatLocationFullCallback(format_location_full_callback_t cb,
                                                                 gpointer user_data) {
  return RegisterCallback("format-location-full", G_CALLBACK(cb), user_data);
}
#endif

void ElementSplitMuxSink::SetSink(Element* sink) {
  SetProperty("sink", sink->GetGstElement());
}

void ElementSplitMuxSink::SetMuxer(Element* muxer) {
  SetProperty("muxer", muxer->GetGstElement());
}

void ElementSplitMuxSink::SetMaxSizeTime(guint64 val) {
  SetProperty("max-size-time", val);
}

void ElementSplitMuxSink::SetSendKeyFrameRequest(gboolean send) {
  SetProperty("send-keyframe-requests", send);
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
