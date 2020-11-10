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

// for element_id_t, device_output_t, deckl...

#include "stream/elements/element.h"    // for Element (ptr only), SupportedElement...
#include "stream/elements/sink/sink.h"  // for ElementBaseSink
#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

typedef ElementBaseSink<ELEMENT_VIDEO_SCREEN_SINK> ElementVideoScreenSink;
typedef ElementBaseSink<ELEMENT_AUDIO_SCREEN_SINK> ElementAudioScreenSink;

ElementVideoScreenSink* make_video_screen_sink(element_id_t sink_id);
ElementAudioScreenSink* make_audio_screen_sink(element_id_t sink_id);

class ElementVideoDeckSink : public ElementBaseSink<ELEMENT_VIDEO_DECK_SINK> {
 public:
  typedef ElementBaseSink<ELEMENT_VIDEO_DECK_SINK> base_class;
  using base_class::base_class;

  void SetMode(decklink_video_mode_t mode = 1);  // 0 - 30 Default: 1, "ntsc"
};

typedef ElementBaseSink<ELEMENT_AUDIO_DECK_SINK> ElementAudioDeckSink;

ElementVideoDeckSink* make_video_deck_sink(element_id_t sink_id);
ElementAudioDeckSink* make_audio_deck_sink(element_id_t sink_id);

Element* make_video_device_sink(SinkDeviceType dev, element_id_t sink_id);
Element* make_audio_device_sink(SinkDeviceType dev, element_id_t sink_id);

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
