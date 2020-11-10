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

#include "stream/elements/sink/screen.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

void ElementVideoDeckSink::SetMode(decklink_video_mode_t mode) {
  SetProperty("mode", mode);
}

ElementVideoScreenSink* make_video_screen_sink(element_id_t sink_id) {
  ElementVideoScreenSink* video_sink = make_video_sink<ElementVideoScreenSink>(sink_id);
  return video_sink;
}

ElementAudioScreenSink* make_audio_screen_sink(element_id_t sink_id) {
  ElementAudioScreenSink* audio_sink = make_audio_sink<ElementAudioScreenSink>(sink_id);
  return audio_sink;
}

ElementVideoDeckSink* make_video_deck_sink(element_id_t sink_id) {
  ElementVideoDeckSink* video_sink = make_video_sink<ElementVideoDeckSink>(sink_id);
  return video_sink;
}

ElementAudioDeckSink* make_audio_deck_sink(element_id_t sink_id) {
  ElementAudioDeckSink* audio_sink = make_audio_sink<ElementAudioDeckSink>(sink_id);
  return audio_sink;
}

Element* make_video_device_sink(SinkDeviceType dev, element_id_t sink_id) {
  if (dev == SCREEN_OUTPUT) {
    return make_video_screen_sink(sink_id);
  } else if (dev == DECKLINK_OUTPUT) {
    return make_video_deck_sink(sink_id);
  }

  return nullptr;
}

Element* make_audio_device_sink(SinkDeviceType dev, element_id_t sink_id) {
  if (dev == SCREEN_OUTPUT) {
    return make_audio_screen_sink(sink_id);
  } else if (dev == DECKLINK_OUTPUT) {
    return make_audio_deck_sink(sink_id);
  }

  return nullptr;
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
