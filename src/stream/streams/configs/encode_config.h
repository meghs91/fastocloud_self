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

#include <string>

#if defined(MACHINE_LEARNING)
#include "base/machine_learning/deep_learning.h"
#include "base/machine_learning/deep_learning_overlay.h"
#endif
#include "base/logo.h"
#include "base/rsvg_logo.h"

#include "stream/streams/configs/audio_video_config.h"

namespace fastocloud {
namespace stream {
namespace streams {

class EncodeConfig : public AudioVideoConfig {
 public:
  typedef AudioVideoConfig base_class;
  typedef common::Optional<Logo> logo_t;
  typedef common::Optional<RSVGLogo> rsvg_logo_t;
#if defined(MACHINE_LEARNING)
  typedef common::Optional<machine_learning::DeepLearning> deep_learning_t;
  typedef common::Optional<machine_learning::DeepLearningOverlay> deep_learning_overlay_t;
#endif

  explicit EncodeConfig(const base_class& config);

  bool GetRelayVideo() const;
  void SetRelayVideo(bool rv);

  bool GetRelayAudio() const;
  void SetRelayAudio(bool ra);

  volume_t GetVolume() const;  // encoding
  void SetVolume(volume_t volume);

  frame_rate_t GetFramerate() const;  // encoding
  void SetFrameRate(frame_rate_t rate);

  deinterlace_t GetDeinterlace() const;  // encoding
  void SetDeinterlace(deinterlace_t deinterlace);

  std::string GetVideoEncoder() const;  // encoding
  void SetVideoEncoder(const std::string& enc);

  std::string GetAudioEncoder() const;  // encoding
  void SetAudioEncoder(const std::string& enc);

  bool IsGpu() const;     // encoding
  bool IsMfxGpu() const;  // encoding

  audio_channels_count_t GetAudioChannelsCount() const;  // encoding
  void SetAudioChannelsCount(audio_channels_count_t channels);

  video_encoders_args_t GetVideoEncoderArgs() const;  // encoding
  void SetVideoEncoderArgs(const video_encoders_args_t& args);

  video_encoders_str_args_t GetVideoEncoderStrArgs() const;  // encoding
  void SetVideoEncoderStrArgs(const video_encoders_str_args_t& args);

  common::draw::Size GetSize() const;  // encoding
  void SetSize(common::draw::Size size);

  bit_rate_t GetVideoBitrate() const;  // encoding
  void SetVideoBitrate(bit_rate_t bitr);

  bit_rate_t GetAudioBitrate() const;  // encoding
  void SetAudioBitrate(bit_rate_t bitr);

  logo_t GetLogo() const;  // encoding
  void SetLogo(const logo_t& logo);

  rsvg_logo_t GetRSVGLogo() const;  // encoding
  void SetRSVGLogo(const rsvg_logo_t& logo);

#if defined(MACHINE_LEARNING)
  deep_learning_t GetDeepLearning() const;  // encoding
  void SetDeepLearning(const deep_learning_t& learning);

  deep_learning_overlay_t GetDeepLearningOverlay() const;  // encoding
  void SetDeepLearningOverlay(const deep_learning_overlay_t& learning);
#endif

  rational_t GetAspectRatio() const;  // encoding
  void SetAspectRatio(rational_t rat);

  decklink_video_mode_t GetDecklinkMode() const;  // mosaic
  void SetDecklinkMode(decklink_video_mode_t decl);

  EncodeConfig* Clone() const override;

 private:
  deinterlace_t deinterlace_;

  frame_rate_t frame_rate_;
  volume_t volume_;

  std::string video_encoder_;
  std::string audio_encoder_;

  audio_channels_count_t audio_channels_count_;

  video_encoders_args_t video_encoder_args_;
  video_encoders_str_args_t video_encoder_str_args_;

  common::draw::Size size_;
  bit_rate_t video_bit_rate_;
  bit_rate_t audio_bit_rate_;

  logo_t logo_;
  rsvg_logo_t rsvg_logo_;
#if defined(MACHINE_LEARNING)
  deep_learning_t learning_;
  deep_learning_overlay_t learning_overlay_;
#endif

  decklink_video_mode_t decklink_video_mode_;
  rational_t aspect_ratio_;

  bool relay_video_;
  bool relay_audio_;
};

class VodEncodeConfig : public EncodeConfig {
 public:
  typedef EncodeConfig base_class;
  explicit VodEncodeConfig(const base_class& config);

  bool GetCleanupTS() const;
  void SetCleanupTS(bool cleanup);

  VodEncodeConfig* Clone() const override;

 private:
  bool cleanup_ts_;
};

typedef EncodeConfig CodEncodeConfig;
typedef EncodeConfig PlaylistEncodeConfig;

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
