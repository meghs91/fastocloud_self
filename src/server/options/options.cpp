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

#include "server/options/options.h"

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <common/draw/size.h>
#include <common/file_system/path.h>
#include <common/media/types.h>

#include "base/config_fields.h"
#include "base/gst_constants.h"
#include "base/types.h"

namespace fastocloud {

namespace {

enum Validity { VALID, INVALID, FATAL };

typedef Validity (*validate_callback_t)(const common::Value*);

typedef std::pair<std::string, validate_callback_t> option_t;

Validity dont_validate(const common::Value*) {
  return Validity::VALID;
}

Validity validate_range(const common::Value* value, double min, double max, bool is_fatal) {
  double i;
  if (value->GetAsDouble(&i) && i >= min && i <= max) {
    return Validity::VALID;
  }

  return is_fatal ? Validity::FATAL : Validity::INVALID;
}

Validity validate_range(const common::Value* value, int min, int max, bool is_fatal) {
  int i;
  if (value->GetAsInteger(&i) && i >= min && i <= max) {
    return Validity::VALID;
  }

  return is_fatal ? Validity::FATAL : Validity::INVALID;
}

Validity validate_is_positive(const common::Value* value, bool is_fatal) {
  int i;
  if (value->GetAsInteger(&i) && i >= 0) {
    return Validity::VALID;
  }

  return is_fatal ? Validity::FATAL : Validity::INVALID;
}

Validity validate_id(const common::Value* value) {
  fastotv::stream_id_t id;
  if (!value->GetAsBasicString(&id)) {
    return Validity::INVALID;
  }

  return id.empty() ? Validity::INVALID : Validity::VALID;
}

Validity validate_input(const common::Value* value) {
  const common::ArrayValue* input = nullptr;
  if (!value->GetAsList(&input)) {
    return Validity::INVALID;
  }

  return Validity::VALID;
}

Validity validate_output(const common::Value* value) {
  const common::ArrayValue* output = nullptr;
  if (!value->GetAsList(&output)) {
    return Validity::INVALID;
  }

  return Validity::VALID;
}

Validity validate_restart_attempts(const common::Value* value) {
  return validate_range(value, 1, std::numeric_limits<int>::max(), false);
}

Validity validate_feedback_dir(const common::Value* value) {
  std::string path;
  if (!value->GetAsBasicString(&path)) {
    return Validity::INVALID;
  }

  return common::file_system::is_valid_path(path) ? Validity::VALID : Validity::INVALID;
}

Validity validate_data_dir(const common::Value* value) {
  std::string path;
  if (!value->GetAsBasicString(&path)) {
    return Validity::INVALID;
  }

  return common::file_system::is_valid_path(path) ? Validity::VALID : Validity::INVALID;
}

Validity validate_timeshift_dir(const common::Value* value) {
  std::string path;
  if (!value->GetAsBasicString(&path)) {
    return Validity::INVALID;
  }

  return common::file_system::is_valid_path(path) ? Validity::VALID : Validity::FATAL;
}

Validity validate_timeshift_chunk_life_time(const common::Value* value) {
  return validate_range(value, 0, 12 * 24 * 3600, false);
}

Validity validate_timeshift_delay(const common::Value* value) {
  return validate_range(value, 0, 12 * 24 * 3600, false);
}

Validity validate_video_parser(const common::Value* value) {
  std::string parser_str;
  if (!value->GetAsBasicString(&parser_str)) {
    return Validity::INVALID;
  }

  for (size_t i = 0; i < SUPPORTED_VIDEO_PARSERS_COUNT; ++i) {
    const char* parser = kSupportedVideoParsers[i];
    if (parser_str == parser) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_audio_parser(const common::Value* value) {
  std::string parser_str;
  if (!value->GetAsBasicString(&parser_str)) {
    return Validity::INVALID;
  }

  for (size_t i = 0; i < SUPPORTED_AUDIO_PARSERS_COUNT; ++i) {
    const char* parser = kSupportedAudioParsers[i];
    if (parser_str == parser) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_video_codec(const common::Value* value) {
  std::string codec_str;
  if (!value->GetAsBasicString(&codec_str)) {
    return Validity::INVALID;
  }

  for (size_t i = 0; i < SUPPORTED_VIDEO_ENCODERS_COUNT; ++i) {
    const char* codec = kSupportedVideoEncoders[i];
    if (codec_str == codec) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_audio_codec(const common::Value* value) {
  std::string codec_str;
  if (!value->GetAsBasicString(&codec_str)) {
    return Validity::INVALID;
  }

  for (size_t i = 0; i < SUPPORTED_AUDIO_ENCODERS_COUNT; ++i) {
    const char* codec = kSupportedAudioEncoders[i];
    if (codec_str == codec) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_type(const common::Value* value) {
  return validate_range(value, static_cast<int>(fastotv::PROXY), static_cast<int>(fastotv::SCREEN), true);
}

Validity validate_log_level(const common::Value* value) {
  return validate_range(value, static_cast<int>(common::logging::LOG_LEVEL_EMERG),
                        static_cast<int>(common::logging::LOG_LEVEL_DEBUG), false);
}

Validity validate_volume(const common::Value* value) {
  return validate_range(value, 0.0, 10.0, false);
}

Validity validate_delay_time(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_timeshift_chunk_duration(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_auto_exit_time(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_size(const common::Value* value) {
  std::string size_str;
  if (!value->GetAsBasicString(&size_str)) {
    return Validity::INVALID;
  }

  common::draw::Size size;
  return common::ConvertFromString(size_str, &size) ? Validity::VALID : Validity::INVALID;
}

Validity validate_cleanupts(const common::Value* value) {
  bool cleanup;
  if (!value->GetAsBoolean(&cleanup)) {
    return Validity::INVALID;
  }

  return Validity::VALID;
}

Validity validate_framerate(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_aspect_ratio(const common::Value* value) {
  std::string rat_str;
  if (!value->GetAsBasicString(&rat_str)) {
    return Validity::INVALID;
  }

  common::media::Rational rat;
  return common::ConvertFromString(rat_str, &rat) ? Validity::VALID : Validity::INVALID;
}

Validity validate_decklink_video_mode(const common::Value* value) {
  return validate_range(value, 0, 30, false);
}

Validity validate_video_bitrate(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_audio_bitrate(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_audio_channels(const common::Value* value) {
  return validate_is_positive(value, false);
}

Validity validate_audio_select(const common::Value* value) {
  int ais;
  if (!value->GetAsInteger(&ais)) {
    return Validity::INVALID;
  }
  return Validity::VALID;
}

Validity validate_mfxh264_preset(const common::Value* value) {
  return validate_range(value, 0, 7, false);
}

Validity validate_mfxh264_gopsize(const common::Value* value) {
  return validate_range(value, 0, 65535, false);
}

Validity validate_nvh264_preset(const common::Value* value) {
  return validate_range(value, 0, std::numeric_limits<int>::max(), false);
}

Validity validate_nvh265_preset(const common::Value* value) {
  return validate_range(value, 0, std::numeric_limits<int>::max(), false);
}

// x264enc validators

Validity validate_x264_speed_preset(const common::Value* value) {
  return validate_range(value, 0, 10, false);
}

Validity validate_x264_threads(const common::Value* value) {
  return validate_range(value, 0, std::numeric_limits<int>::max(), false);
}

Validity validate_x264_tune(const common::Value* value) {
  static const int allowed_values[] = {0x0, 0x1, 0x2, 0x4};
  int tune;
  if (!value->GetAsInteger(&tune)) {
    return Validity::INVALID;
  }

  for (auto i : allowed_values) {
    if (i == tune) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_x264_key_int_max(const common::Value* value) {
  return validate_range(value, 0, std::numeric_limits<int>::max(), false);
}

Validity validate_x264_vbv_buf_capacity(const common::Value* value) {
  return validate_range(value, 0, 10000, false);
}

Validity validate_x264_rc_lookahead(const common::Value* value) {
  return validate_range(value, 0, 250, false);
}

Validity validate_x264_qp_max(const common::Value* value) {
  return validate_range(value, 0, 51, false);
}

Validity validate_x264_pass(const common::Value* value) {
  static const int allowed_values[] = {0, 4, 5, 17, 18, 19};
  int pass;
  if (value->GetAsInteger(&pass)) {
    return Validity::INVALID;
  }

  for (const auto& i : allowed_values) {
    if (i == pass) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_x264_me(const common::Value* value) {
  return validate_range(value, 0, 4, false);
}

// vaapih264 validators

Validity validate_vaapih264_keyframe_period(const common::Value* value) {
  return validate_range(value, 0, 300, false);
}

Validity validate_vaapih264_tune(const common::Value* value) {
  static const int allowed_values[] = {0, 1, 3};
  int tune;
  if (value->GetAsInteger(&tune)) {
    return Validity::INVALID;
  }
  for (const auto& i : allowed_values) {
    if (i == tune) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_vaapih264_max_bframes(const common::Value* value) {
  return validate_range(value, 0, 10, false);
}

Validity validate_vaapih264_num_slices(const common::Value* value) {
  return validate_range(value, 1, 200, false);
}

Validity validate_vaapih264_init_qp(const common::Value* value) {
  return validate_range(value, 1, 51, false);
}

Validity validate_vaapih264_min_qp(const common::Value* value) {
  return validate_range(value, 1, 51, false);
}

Validity validate_vaapih264_rate_control(const common::Value* value) {
  static const int allowed_values[] = {1, 2, 4, 5};
  int rate_control;
  if (value->GetAsInteger(&rate_control)) {
    return Validity::INVALID;
  }

  for (const auto& i : allowed_values) {
    if (i == rate_control) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_vaapih264_cpb_length(const common::Value* value) {
  return validate_range(value, 0, 10000, false);
}

Validity dummy_validator_integer(const common::Value* value) {
  return validate_range(value, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
}

Validity dummy_validator_string(const common::Value* value) {
  std::string not_empty;
  if (!value->GetAsBasicString(&not_empty)) {
    return Validity::INVALID;
  }

  return not_empty.empty() ? Validity::INVALID : Validity::VALID;
}

class ConstantOptions : public std::vector<option_t> {
 public:
  ConstantOptions(std::initializer_list<option_t> l) {
    for (auto it = l.begin(); it != l.end(); ++it) {
      option_t opt = *it;
      for (auto jt = begin(); jt != end(); ++jt) {
        option_t opt2 = *jt;
        if (opt2.first == opt.first) {
          NOTREACHED() << "Only unique options, but option with name: '" << opt.first << "' exists!";
        }
      }
      push_back(opt);
    }
  }
};

bool FindOption(const std::string& key, option_t* opt) {
  if (!opt) {
    return false;
  }

  static const ConstantOptions ALLOWED_OPTIONS = {
    {ID_FIELD, validate_id},
    {TYPE_FIELD, validate_type},
    {FEEDBACK_DIR_FIELD, validate_feedback_dir},
    {DATA_DIR_FIELD, validate_data_dir},
    {LOG_LEVEL_FIELD, validate_log_level},
    {STREAM_LINK_PATH_FIELD, dont_validate},
    {INPUT_FIELD, validate_input},
    {OUTPUT_FIELD, validate_output},
    {RESTART_ATTEMPTS_FIELD, validate_restart_attempts},
    {AUTO_EXIT_TIME_FIELD, validate_auto_exit_time},
    {TIMESHIFT_DIR_FIELD, validate_timeshift_dir},
    {TIMESHIFT_CHUNK_LIFE_TIME_FIELD, validate_timeshift_chunk_life_time},
    {TIMESHIFT_DELAY_FIELD, validate_timeshift_delay},
    {MAIN_PROFILE_FIELD, dont_validate},
    {MAIN_PROFILE_EXTERNAL_FIELD, dont_validate},
    {VOLUME_FIELD, validate_volume},
    {DELAY_TIME_FIELD, validate_delay_time},
    {TIMESHIFT_CHUNK_DURATION_FIELD, validate_timeshift_chunk_duration},
    {VIDEO_PARSER_FIELD, validate_video_parser},
    {AUDIO_PARSER_FIELD, validate_audio_parser},
    {AUDIO_CODEC_FIELD, validate_audio_codec},
    {VIDEO_CODEC_FIELD, validate_video_codec},
    {HAVE_VIDEO_FIELD, dont_validate},
    {HAVE_AUDIO_FIELD, dont_validate},
    {HAVE_SUBTITLE_FIELD, dont_validate},
    {DEINTERLACE_FIELD, dont_validate},
    {RELAY_AUDIO_FIELD, dont_validate},
    {RELAY_VIDEO_FIELD, dont_validate},
    {LOOP_FIELD, dont_validate},
    {SIZE_FIELD, validate_size},
    {CLEANUP_TS_FIELD, validate_cleanupts},
    {LOGO_FIELD, dont_validate},
    {RSVG_LOGO_FIELD, dont_validate},
    {FRAME_RATE_FIELD, validate_framerate},
    {ASPECT_RATIO_FIELD, validate_aspect_ratio},
    {VIDEO_BIT_RATE_FIELD, validate_video_bitrate},
    {AUDIO_BIT_RATE_FIELD, validate_audio_bitrate},
    {AUDIO_CHANNELS_FIELD, validate_audio_channels},
    {AUDIO_SELECT_FIELD, validate_audio_select},
    {DECKLINK_VIDEO_MODE_FIELD, validate_decklink_video_mode},
#if defined(MACHINE_LEARNING)
    {DEEP_LEARNING_FIELD, dont_validate},
    {DEEP_LEARNING_OVERLAY_FIELD, dont_validate},
#endif
#if defined(AMAZON_KINESIS)
    {AMAZON_KINESIS_FIELD, dont_validate},
#endif
    {NV_H264_ENC_PRESET, validate_nvh264_preset},
    {NV_H265_ENC_PRESET, validate_nvh265_preset},
    {MFX_H264_ENC_PRESET, validate_mfxh264_preset},
    {MFX_H264_GOP_SIZE, validate_mfxh264_gopsize},
    {X264_ENC_SPEED_PRESET, validate_x264_speed_preset},
    {X264_ENC_THREADS, validate_x264_threads},
    {X264_ENC_TUNE, validate_x264_tune},
    {X264_ENC_KEY_INT_MAX, validate_x264_key_int_max},
    {X264_ENC_VBV_BUF_CAPACITY, validate_x264_vbv_buf_capacity},
    {X264_ENC_RC_LOOKAHED, validate_x264_rc_lookahead},
    {X264_ENC_QP_MAX, validate_x264_qp_max},
    {X264_ENC_PASS, validate_x264_pass},
    {X264_ENC_ME, validate_x264_me},
    {X264_ENC_PROFILE, dummy_validator_string},
    {X264_ENC_STREAM_FORMAT, dummy_validator_string},
    {X264_ENC_OPTION_STRING, dont_validate},
    {X264_ENC_INTERLACED, dont_validate},
    {X264_ENC_DCT8X8, dont_validate},
    {X264_ENC_B_ADAPT, dont_validate},
    {X264_ENC_BYTE_STREAM, dont_validate},
    {X264_ENC_CABAC, dont_validate},
    {X264_ENC_SLICED_THREADS, dont_validate},
    {X264_ENC_QUANTIZER, dont_validate},
    {VAAPI_H264_ENC_KEYFRAME_PERIOD, validate_vaapih264_keyframe_period},
    {VAAPI_H264_ENC_TUNE, validate_vaapih264_tune},
    {VAAPI_H264_ENC_MAX_BFRAMES, validate_vaapih264_max_bframes},
    {VAAPI_H264_ENC_NUM_SLICES, validate_vaapih264_num_slices},
    {VAAPI_H264_ENC_INIT_QP, validate_vaapih264_init_qp},
    {VAAPI_H264_ENC_MIN_QP, validate_vaapih264_min_qp},
    {VAAPI_H264_ENC_RATE_CONTROL, validate_vaapih264_rate_control},
    {VAAPI_H264_ENC_CABAC, dont_validate},
    {VAAPI_H264_ENC_DCT8X8, dont_validate},
    {VAAPI_H264_ENC_CPB_LENGTH, validate_vaapih264_cpb_length},
    {OPEN_H264_ENC_MUTLITHREAD, dont_validate},
    {OPEN_H264_ENC_COMPLEXITY, dummy_validator_integer},
    {OPEN_H264_ENC_RATE_CONTROL, dummy_validator_integer},
    {OPEN_H264_ENC_GOP_SIZE, dummy_validator_integer},
    {EAVC_ENC_PRESET, dummy_validator_integer},
    {EAVC_ENC_PROFILE, dummy_validator_integer},
    {EAVC_ENC_PERFORMANCE, dummy_validator_integer},
    {EAVC_ENC_BITRATE_MODE, dummy_validator_integer},
    {EAVC_ENC_BITRATE_PASS, dummy_validator_integer},
    {EAVC_ENC_BITRATE_MAX, dummy_validator_integer},
    {EAVC_ENC_VBV_SIZE, dummy_validator_integer},
    {EAVC_ENC_PICTURE_MODE, dummy_validator_integer},
    {EAVC_ENC_ENTROPY_MODE, dummy_validator_integer},
    {EAVC_ENC_GOP_MAX_BCOUNT, dummy_validator_integer},
    {EAVC_ENC_GOP_MAX_LENGTH, dummy_validator_integer},
    {EAVC_ENC_GOP_MIN_LENGTH, dummy_validator_integer},
    {EAVC_ENC_LEVEL, dummy_validator_integer},
    {EAVC_ENC_DEBLOCK_MODE, dummy_validator_integer},
    {EAVC_ENC_DEBLOCK_ALPHA, dummy_validator_integer},
    {EAVC_ENC_DEBLOCK_BETA, dummy_validator_integer},
    {EAVC_ENC_INITIAL_DELAY, dummy_validator_integer},
    {EAVC_ENC_FIELD_ORDER, dummy_validator_integer},
    {EAVC_ENC_GOP_ADAPTIVE, dont_validate},
    {TS_DEMUX_PROGRAM_NUMBER, dont_validate},
  };
  for (const option_t& cur : ALLOWED_OPTIONS) {
    if (cur.first == key) {
      *opt = cur;
      return true;
    }
  }

  return false;
}

}  // namespace

namespace server {
namespace options {

common::ErrnoError ValidateConfig(const StreamConfig& config) {
  if (!config) {
    return common::make_errno_error_inval();
  }

  for (auto it = config->begin(); it != config->end(); ++it) {
    option_t option;
    std::string key = it->first.as_string();
    if (!FindOption(key, &option)) {
      WARNING_LOG() << "Unknown option: " << key;
    } else {
      common::Value* value = it->second;
      Validity valid = option.second(value);
      if (valid == Validity::INVALID) {
        WARNING_LOG() << "Invalid value '" << value << "' of option '" << key << "'";
      } else if (valid == Validity::FATAL) {
        std::stringstream os;
        os << "Invalid value '" << value << "' of option '" << key << "'";
        ERROR_LOG() << os.str();
        return common::make_errno_error(os.str(), EINVAL);
      }
    }
  }

  return common::ErrnoError();
}

}  // namespace options
}  // namespace server
}  // namespace fastocloud
