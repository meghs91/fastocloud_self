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

#include <glib.h>
#include <gst/gstversion.h>

#include <common/sprintf.h>

#include "stream/elements/element.h"
#include "stream/stypes.h"

#if GST_CHECK_VERSION(1, 11, 1)
typedef struct _GstSample GstSample;
#endif

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

class ElementMultiFileSink : public ElementEx<ELEMENT_MULTIFILE_SINK> {
 public:
  typedef ElementEx<ELEMENT_MULTIFILE_SINK> base_class;
  using base_class::base_class;
};

class ElementSplitMuxSink : public ElementEx<ELEMENT_SPLIT_MUX_SINK> {
 public:
  typedef ElementEx<ELEMENT_SPLIT_MUX_SINK> base_class;
  typedef gchararray (*format_location_callback_t)(GstElement* splitmux, guint fragment_id, gpointer user_data);
#if GST_CHECK_VERSION(1, 11, 1)
  typedef gchararray (*format_location_full_callback_t)(GstElement* splitmux,
                                                        guint fragment_id,
                                                        GstSample* sample,
                                                        gpointer user_data);
#endif

  using base_class::base_class;
  gboolean RegisterFormatLocationCallback(format_location_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
#if GST_CHECK_VERSION(1, 11, 1)
  gboolean RegisterFormatLocationFullCallback(format_location_full_callback_t cb,
                                              gpointer user_data) WARN_UNUSED_RESULT;
#endif

  void SetSink(Element* sink);
  void SetMuxer(Element* muxer);
  void SetMaxSizeTime(guint64 val = 0);        // 0 - 18446744073709551615 Default: 0
  void SetSendKeyFrameRequest(gboolean send);  // Default: false
};

template <SupportedElements el>
class ElementBaseSink : public ElementEx<el> {
 public:
  typedef ElementEx<el> base_class;
  using base_class::base_class;

  void SetSync(bool sync) { ElementEx<el>::SetProperty("sync", sync); }
};

template <typename T>
T* make_video_sink(element_id_t parser_id) {
  return make_element<T>(common::MemSPrintf(VIDEO_SINK_NAME_1U, parser_id));
}

template <typename T>
T* make_audio_sink(element_id_t parser_id) {
  return make_element<T>(common::MemSPrintf(AUDIO_SINK_NAME_1U, parser_id));
}

template <typename T>
T* make_sink(element_id_t parser_id) {
  return make_element<T>(common::MemSPrintf(SINK_NAME_1U, parser_id));
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
