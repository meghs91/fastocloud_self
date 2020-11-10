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

#include "stream/configs_factory.h"

#include <map>
#include <string>

#include <common/sprintf.h>

#include "base/config_fields.h"
#include "base/gst_constants.h"

#include "stream/link_generator/ilink_generator.h"
#include "stream/streams/configs/encode_config.h"
#include "stream/streams/configs/relay_config.h"

namespace fastocloud {
namespace stream {

namespace {

const size_t kDefaultRestartAttempts = 10;

void CheckAndSetValue(const StreamConfig& config, const std::string& name, video_encoders_args_t* map) {
  if (!config || !map) {
    return;
  }

  int result;
  common::Value* setting = config->Find(name);
  if (setting && setting->GetAsInteger(&result)) {
    map->insert(std::make_pair(name, result));
  }
}

void CheckAndSetValue(const StreamConfig& config, const std::string& name, video_encoders_str_args_t* map) {
  if (!config || !map) {
    return;
  }

  std::string result;
  common::Value* setting = config->Find(name);
  if (setting && setting->GetAsBasicString(&result)) {
    map->insert(std::make_pair(name, result));
  }
}

bool InitVideoEncodersWithArgs(const StreamConfig& config,
                               video_encoders_args_t* video_encoder_args,
                               video_encoders_str_args_t* video_encoder_str_args) {
  if (!video_encoder_args || !video_encoder_str_args) {
    return false;
  }

  video_encoders_args_t video_encoder_args_;
  video_encoders_str_args_t video_encoder_str_args_;

  // nvh264enc
  CheckAndSetValue(config, NV_H264_ENC_PRESET, &video_encoder_args_);
  // nvh265enc
  CheckAndSetValue(config, NV_H265_ENC_PRESET, &video_encoder_args_);

  // mfxh264enc
  CheckAndSetValue(config, MFX_H264_ENC_PRESET, &video_encoder_args_);
  CheckAndSetValue(config, MFX_H264_GOP_SIZE, &video_encoder_args_);

  // x264enc
  CheckAndSetValue(config, X264_ENC_SPEED_PRESET, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_THREADS, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_TUNE, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_KEY_INT_MAX, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_VBV_BUF_CAPACITY, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_RC_LOOKAHED, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_QP_MAX, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_PASS, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_ME, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_SLICED_THREADS, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_B_ADAPT, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_BYTE_STREAM, &video_encoder_args_);
  CheckAndSetValue(config, X264_ENC_QUANTIZER, &video_encoder_args_);

  CheckAndSetValue(config, X264_ENC_PROFILE, &video_encoder_str_args_);
  CheckAndSetValue(config, X264_ENC_STREAM_FORMAT, &video_encoder_str_args_);
  CheckAndSetValue(config, X264_ENC_OPTION_STRING, &video_encoder_str_args_);

  bool x264_enc_inter = false;
  common::Value* x264_enc_inter_field = config->Find(X264_ENC_INTERLACED);
  if (x264_enc_inter_field && x264_enc_inter_field->GetAsBoolean(&x264_enc_inter) && x264_enc_inter) {
    video_encoder_args_[X264_ENC_INTERLACED] = 1;
  }

  bool x264_enc_dct = false;
  common::Value* x264_enc_dct_field = config->Find(X264_ENC_DCT8X8);
  if (x264_enc_dct_field && x264_enc_dct_field->GetAsBoolean(&x264_enc_dct) && x264_enc_dct) {
    video_encoder_args_[X264_ENC_DCT8X8] = 1;
  }

  // vaapih264enc
  CheckAndSetValue(config, VAAPI_H264_ENC_KEYFRAME_PERIOD, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_TUNE, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_MAX_BFRAMES, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_NUM_SLICES, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_INIT_QP, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_MIN_QP, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_RATE_CONTROL, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_CABAC, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_DCT8X8, &video_encoder_args_);
  CheckAndSetValue(config, VAAPI_H264_ENC_CPB_LENGTH, &video_encoder_args_);

  // openh264enc
  CheckAndSetValue(config, OPEN_H264_ENC_MUTLITHREAD, &video_encoder_args_);
  CheckAndSetValue(config, OPEN_H264_ENC_COMPLEXITY, &video_encoder_args_);
  CheckAndSetValue(config, OPEN_H264_ENC_RATE_CONTROL, &video_encoder_args_);
  CheckAndSetValue(config, OPEN_H264_ENC_GOP_SIZE, &video_encoder_args_);

  // eavcenc
  CheckAndSetValue(config, EAVC_ENC_PRESET, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_PROFILE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_PERFORMANCE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_BITRATE_MODE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_BITRATE_PASS, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_BITRATE_MAX, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_VBV_SIZE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_PICTURE_MODE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_ENTROPY_MODE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_GOP_MAX_BCOUNT, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_GOP_MAX_LENGTH, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_GOP_MIN_LENGTH, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_LEVEL, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_DEBLOCK_MODE, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_DEBLOCK_ALPHA, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_DEBLOCK_BETA, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_INITIAL_DELAY, &video_encoder_args_);
  CheckAndSetValue(config, EAVC_ENC_FIELD_ORDER, &video_encoder_args_);
  bool eabc_enc_gop = false;
  common::Value* eabc_enc_gop_field = config->Find(EAVC_ENC_GOP_ADAPTIVE);
  if (eabc_enc_gop_field && eabc_enc_gop_field->GetAsBoolean(&eabc_enc_gop) && eabc_enc_gop) {
    video_encoder_args_[EAVC_ENC_GOP_ADAPTIVE] = 1;
  }

  *video_encoder_args = video_encoder_args_;
  *video_encoder_str_args = video_encoder_str_args_;
  return true;
}

}  // namespace

common::Error make_config(const StreamConfig& config_args, Config** config) {
  if (!config_args || !config) {
    return common::make_error_inval();
  }

  int type;
  common::Value* type_field = config_args->Find(TYPE_FIELD);
  if (!type_field || !type_field->GetAsInteger(&type)) {
    return common::make_error("Define " TYPE_FIELD " variable and make it valid");
  }

  fastotv::StreamType stream_type = static_cast<fastotv::StreamType>(type);
  if (stream_type == fastotv::PROXY || stream_type == fastotv::VOD_PROXY) {
    return common::make_error("Proxy streams not handled for now");
  }

  input_t input_urls;
  if (!read_input(config_args, &input_urls)) {
    return common::make_error("Define " INPUT_FIELD " variable and make it valid");
  }

  output_t output_urls;
  if (!read_output(config_args, &output_urls)) {
    return common::make_error("Define " OUTPUT_FIELD " variable and make it valid");
  }

  int max_restart_attempts;
  common::Value* restart_attempts_field = config_args->Find(RESTART_ATTEMPTS_FIELD);
  if (!restart_attempts_field || restart_attempts_field->GetAsInteger(&max_restart_attempts)) {
    max_restart_attempts = kDefaultRestartAttempts;
  }
  CHECK(max_restart_attempts > 0) << "restart attempts must be grether than 0";

  Config conf(static_cast<fastotv::StreamType>(stream_type), max_restart_attempts, input_urls, output_urls);

  int ttl_sec;
  common::Value* ttl_sec_field = config_args->Find(AUTO_EXIT_TIME_FIELD);
  if (ttl_sec_field && ttl_sec_field->GetAsInteger(&ttl_sec)) {
    conf.SetTimeToLifeStream(ttl_sec);
  }

  streams::AudioVideoConfig aconf(conf);
  bool have_video;
  common::Value* have_video_field = config_args->Find(HAVE_VIDEO_FIELD);
  if (have_video_field && have_video_field->GetAsBoolean(&have_video)) {
    aconf.SetHaveVideo(have_video);
  }

  bool have_audio;
  common::Value* have_audio_field = config_args->Find(HAVE_AUDIO_FIELD);
  if (have_audio_field && have_audio_field->GetAsBoolean(&have_audio)) {
    aconf.SetHaveAudio(have_audio);
  }

  bool have_subtitle;
  common::Value* have_subtitle_field = config_args->Find(HAVE_SUBTITLE_FIELD);
  if (have_subtitle_field && have_subtitle_field->GetAsBoolean(&have_subtitle)) {
    aconf.SetHaveSubtitle(have_subtitle);
  }

  int audio_select;
  common::Value* audio_select_field = config_args->Find(AUDIO_SELECT_FIELD);
  if (audio_select_field && audio_select_field->GetAsInteger(&audio_select)) {
    aconf.SetAudioSelect(audio_select);
  }

  bool loop;
  common::Value* loop_field = config_args->Find(LOOP_FIELD);
  if (loop_field && loop_field->GetAsBoolean(&loop)) {
    aconf.SetLoop(loop);
  }

  if (stream_type == fastotv::SCREEN) {
    *config = new streams::AudioVideoConfig(aconf);
    return common::Error();
  } else if (stream_type == fastotv::RELAY || stream_type == fastotv::TIMESHIFT_PLAYER ||
             stream_type == fastotv::TEST_LIFE || stream_type == fastotv::VOD_RELAY ||
             stream_type == fastotv::COD_RELAY) {
    streams::RelayConfig* rconfig = new streams::RelayConfig(aconf);

    std::string video_parser;
    common::Value* video_parser_field = config_args->Find(VIDEO_PARSER_FIELD);
    if (video_parser_field && video_parser_field->GetAsBasicString(&video_parser)) {
      rconfig->SetVideoParser(video_parser);
    }

    std::string audio_parser;
    common::Value* audio_parser_field = config_args->Find(AUDIO_PARSER_FIELD);
    if (audio_parser_field && audio_parser_field->GetAsBasicString(&audio_parser)) {
      rconfig->SetAudioParser(audio_parser);
    }

    if (stream_type == fastotv::VOD_RELAY) {
      streams::VodRelayConfig* vconf = new streams::VodRelayConfig(*rconfig);
      delete rconfig;
      rconfig = vconf;
      bool cleanup_ts;
      common::Value* cleanup_ts_field = config_args->Find(CLEANUP_TS_FIELD);
      if (cleanup_ts_field && cleanup_ts_field->GetAsBoolean(&cleanup_ts)) {
        vconf->SetCleanupTS(cleanup_ts);
      }
    }

    *config = rconfig;
    return common::Error();
  } else if (stream_type == fastotv::ENCODE || stream_type == fastotv::VOD_ENCODE ||
             stream_type == fastotv::COD_ENCODE || stream_type == fastotv::EVENT) {
    streams::EncodeConfig* econfig = new streams::EncodeConfig(aconf);
    bool relay_audio;
    common::Value* relay_audio_field = config_args->Find(RELAY_AUDIO_FIELD);
    if (relay_audio_field && relay_audio_field->GetAsBoolean(&relay_audio)) {
      econfig->SetRelayAudio(relay_audio);
    }

    bool relay_video;
    common::Value* relay_video_field = config_args->Find(RELAY_VIDEO_FIELD);
    if (relay_video_field && relay_video_field->GetAsBoolean(&relay_video)) {
      econfig->SetRelayVideo(relay_video);
    }

    bool deinterlace;
    common::Value* deinterlace_field = config_args->Find(DEINTERLACE_FIELD);
    if (deinterlace_field && deinterlace_field->GetAsBoolean(&deinterlace)) {
      econfig->SetDeinterlace(deinterlace);
    }

    int frame_rate;
    common::Value* frame_rate_field = config_args->Find(FRAME_RATE_FIELD);
    if (frame_rate_field && frame_rate_field->GetAsInteger(&frame_rate)) {
      econfig->SetFrameRate(frame_rate);
    }

    double volume;
    common::Value* volume_field = config_args->Find(VOLUME_FIELD);
    if (volume_field && volume_field->GetAsDouble(&volume)) {
      econfig->SetVolume(volume);
    }

    std::string video_codec;
    common::Value* video_codec_field = config_args->Find(VIDEO_CODEC_FIELD);
    if (video_codec_field && video_codec_field->GetAsBasicString(&video_codec)) {
      econfig->SetVideoEncoder(video_codec);
    }

    std::string audio_codec;
    common::Value* audio_codec_field = config_args->Find(AUDIO_CODEC_FIELD);
    if (audio_codec_field && audio_codec_field->GetAsBasicString(&audio_codec)) {
      econfig->SetAudioEncoder(audio_codec);
    }

    int audio_channels;
    common::Value* audio_channels_field = config_args->Find(AUDIO_CHANNELS_FIELD);
    if (audio_channels_field && audio_channels_field->GetAsInteger(&audio_channels)) {
      econfig->SetAudioChannelsCount(audio_channels);
    }

    common::draw::Size size;
    common::Value* size_field = config_args->Find(SIZE_FIELD);
    std::string size_str;
    if (size_field && size_field->GetAsBasicString(&size_str) && common::ConvertFromString(size_str, &size)) {
      econfig->SetSize(size);
    }

    int v_bitrate;
    common::Value* video_bitrate_field = config_args->Find(VIDEO_BIT_RATE_FIELD);
    if (video_bitrate_field && video_bitrate_field->GetAsInteger(&v_bitrate)) {
      econfig->SetVideoBitrate(v_bitrate);
    }

    int a_bitrate;
    common::Value* audio_bitrate_field = config_args->Find(AUDIO_BIT_RATE_FIELD);
    if (audio_bitrate_field && audio_bitrate_field->GetAsInteger(&a_bitrate)) {
      econfig->SetAudioBitrate(a_bitrate);
    }

#if defined(MACHINE_LEARNING)
    common::HashValue* deep_learning_hash = nullptr;
    common::Value* deep_learning_field = config_args->Find(DEEP_LEARNING_FIELD);
    if (deep_learning_field && deep_learning_field->GetAsHash(&deep_learning_hash)) {
      auto deep_learning = machine_learning::DeepLearning::MakeDeepLearning(deep_learning_hash);
      if (deep_learning) {
        econfig->SetDeepLearning(*deep_learning);
      }
    }

    common::HashValue* deep_learning_overlay_hash = nullptr;
    common::Value* deep_learning_overlay_field = config_args->Find(DEEP_LEARNING_OVERLAY_FIELD);
    if (deep_learning_overlay_field && deep_learning_overlay_field->GetAsHash(&deep_learning_overlay_hash)) {
      auto deep_learning_overlay =
          machine_learning::DeepLearningOverlay::MakeDeepLearningOverlay(deep_learning_overlay_hash);
      if (deep_learning_overlay) {
        econfig->SetDeepLearningOverlay(*deep_learning_overlay);
      }
    }
#endif

    common::HashValue* logo_hash = nullptr;
    common::Value* logo_field = config_args->Find(LOGO_FIELD);
    if (logo_field && logo_field->GetAsHash(&logo_hash)) {
      auto logo = Logo::MakeLogo(logo_hash);
      if (logo) {
        econfig->SetLogo(*logo);
      }
    }

    common::HashValue* rsvg_logo_hash = nullptr;
    common::Value* rsvg_logo_field = config_args->Find(RSVG_LOGO_FIELD);
    if (rsvg_logo_field && rsvg_logo_field->GetAsHash(&rsvg_logo_hash)) {
      auto logo = RSVGLogo::MakeLogo(rsvg_logo_hash);
      if (logo) {
        econfig->SetRSVGLogo(*logo);
      }
    }

    common::media::Rational rat;
    common::Value* rat_field = config_args->Find(ASPECT_RATIO_FIELD);
    std::string rat_str;
    if (rat_field && rat_field->GetAsBasicString(&rat_str) && common::ConvertFromString(rat_str, &rat)) {
      econfig->SetAspectRatio(rat);
    }

    int decl_vm;
    common::Value* decl_vm_field = config_args->Find(DECKLINK_VIDEO_MODE_FIELD);
    if (decl_vm_field && decl_vm_field->GetAsInteger(&decl_vm)) {
      econfig->SetDecklinkMode(decl_vm);
    }

    video_encoders_args_t video_encoder_args;
    video_encoders_str_args_t video_encoder_str_args;
    if (InitVideoEncodersWithArgs(config_args, &video_encoder_args, &video_encoder_str_args)) {
      econfig->SetVideoEncoderArgs(video_encoder_args);
      econfig->SetVideoEncoderStrArgs(video_encoder_str_args);
    }

    if (stream_type == fastotv::VOD_ENCODE) {
      streams::VodEncodeConfig* vconf = new streams::VodEncodeConfig(*econfig);
      delete econfig;
      econfig = vconf;
      bool cleanup_ts;
      common::Value* cleanup_ts_field = config_args->Find(CLEANUP_TS_FIELD);
      if (cleanup_ts_field && cleanup_ts_field->GetAsBoolean(&cleanup_ts)) {
        vconf->SetCleanupTS(cleanup_ts);
      }
    }

    *config = econfig;
    return common::Error();
  } else if (stream_type == fastotv::TIMESHIFT_RECORDER || stream_type == fastotv::CATCHUP) {
    streams::RelayConfig rel(aconf);

    std::string video_parser;
    common::Value* video_parser_field = config_args->Find(VIDEO_PARSER_FIELD);
    if (video_parser_field && video_parser_field->GetAsBasicString(&video_parser)) {
      rel.SetVideoParser(video_parser);
    }

    std::string audio_parser;
    common::Value* audio_parser_field = config_args->Find(AUDIO_PARSER_FIELD);
    if (audio_parser_field && audio_parser_field->GetAsBasicString(&audio_parser)) {
      rel.SetAudioParser(audio_parser);
    }

    streams::TimeshiftConfig* tconf = new streams::TimeshiftConfig(rel);
    int timeshift_chunk_duration;
    common::Value* timeshift_chunk_duration_field = config_args->Find(TIMESHIFT_CHUNK_DURATION_FIELD);
    if (timeshift_chunk_duration_field && timeshift_chunk_duration_field->GetAsInteger(&timeshift_chunk_duration)) {
      tconf->SetTimeShiftChunkDuration(timeshift_chunk_duration);
    }
    CHECK(tconf->GetTimeShiftChunkDuration()) << "Avoid division by zero";

    *config = tconf;
    return common::Error();
  }

  return common::make_error(common::MemSPrintf("Unhandled stream type: %d", stream_type));
}

Config* make_config_copy(const Config* conf, const link_generator::ILinkGenerator* generator) {
  Config* copy = conf->Clone();
  if (generator) {
    input_t input = copy->GetUrl();
    for (size_t i = 0; i < input.size(); ++i) {
      InputUri url;
      if (generator->Generate(input[i], &url)) {
        input[i] = url;
        DEBUG_LOG() << "Generated url: " << url.GetUrl().spec();
      }
    }
    copy->SetInput(input);
  }

  return copy;
}

}  // namespace stream
}  // namespace fastocloud
