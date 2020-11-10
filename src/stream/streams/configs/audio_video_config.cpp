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

#include "stream/streams/configs/audio_video_config.h"

#include "base/constants.h"

namespace fastocloud {
namespace stream {
namespace streams {

AudioVideoConfig::AudioVideoConfig(const base_class& config)
    : base_class(config),
      have_video_(true),
      have_audio_(true),
      have_subtitle_(false),
      audio_select_(),
      loop_(DEFAULT_LOOP) {}

AudioVideoConfig::have_stream_t AudioVideoConfig::HaveVideo() const {
  return have_video_;
}

void AudioVideoConfig::SetHaveVideo(have_stream_t have_video) {
  have_video_ = have_video;
}

AudioVideoConfig::have_stream_t AudioVideoConfig::HaveAudio() const {
  return have_audio_;
}

void AudioVideoConfig::SetHaveAudio(have_stream_t have_audio) {
  have_audio_ = have_audio;
}

AudioVideoConfig::have_stream_t AudioVideoConfig::HaveSubtitle() const {
  return have_subtitle_;
}

void AudioVideoConfig::SetHaveSubtitle(have_stream_t have_subtitle) {
  have_subtitle_ = have_subtitle;
}

AudioVideoConfig::audio_select_t AudioVideoConfig::GetAudioSelect() const {
  return audio_select_;
}

void AudioVideoConfig::SetAudioSelect(audio_select_t sel) {
  audio_select_ = sel;
}

AudioVideoConfig::loop_t AudioVideoConfig::GetLoop() const {
  return loop_;
}

void AudioVideoConfig::SetLoop(loop_t loop) {
  loop_ = loop;
}

AudioVideoConfig* AudioVideoConfig::Clone() const {
  return new AudioVideoConfig(*this);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
