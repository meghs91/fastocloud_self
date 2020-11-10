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

#define FEEDBACK_DIR_FIELD "feedback_directory"  // required
#define DATA_DIR_FIELD "data_directory"
#define LOG_LEVEL_FIELD "log_level"
#define ID_FIELD "id"      // required
#define TYPE_FIELD "type"  // required
#define STREAM_LINK_PATH_FIELD "stream_link_path"
#define AUTO_EXIT_TIME_FIELD "auto_exit_time"

#define INPUT_FIELD "input"  // required
#define OUTPUT_FIELD "output"
#define HAVE_VIDEO_FIELD "have_video"
#define HAVE_AUDIO_FIELD "have_audio"
#define HAVE_SUBTITLE_FIELD "have_subtitle"
#define DEINTERLACE_FIELD "deinterlace"
#define FRAME_RATE_FIELD "frame_rate"
#define AUDIO_CHANNELS_FIELD "audio_channels"
#define VOLUME_FIELD "volume"
#define VIDEO_PARSER_FIELD "video_parser"
#define AUDIO_PARSER_FIELD "audio_parser"
#define VIDEO_CODEC_FIELD "video_codec"
#define AUDIO_CODEC_FIELD "audio_codec"
#define AUDIO_SELECT_FIELD "audio_select"
#define TIMESHIFT_DIR_FIELD "timeshift_dir"  // requeired in timeshift mode
#define TIMESHIFT_CHUNK_LIFE_TIME_FIELD "timeshift_chunk_life_time"
#define TIMESHIFT_DELAY_FIELD "timeshift_delay"
#define TIMESHIFT_CHUNK_DURATION_FIELD "timeshift_chunk_duration"
#define CLEANUP_TS_FIELD "cleanup_ts"
#define LOGO_FIELD "logo"
#define RSVG_LOGO_FIELD "rsvg_logo"
#define LOOP_FIELD "loop"
#define RESTART_ATTEMPTS_FIELD "restart_attempts"
#define DELAY_TIME_FIELD "delay_time"
#define SIZE_FIELD "size"
#define VIDEO_BIT_RATE_FIELD "video_bitrate"
#define AUDIO_BIT_RATE_FIELD "audio_bitrate"
#define MAIN_PROFILE_FIELD "mainprofile"
#define MAIN_PROFILE_EXTERNAL_FIELD "mainprofile_external"
#define ASPECT_RATIO_FIELD "aspect_ratio"
#define RELAY_AUDIO_FIELD "relay_audio"
#define RELAY_VIDEO_FIELD "relay_video"

#define DECKLINK_VIDEO_MODE_FIELD "decklink_video_mode"

#if defined(MACHINE_LEARNING)
#define DEEP_LEARNING_FIELD "deep_learning"
#define DEEP_LEARNING_OVERLAY_FIELD "deep_learning_overlay"
#endif

#if defined(AMAZON_KINESIS)
#define AMAZON_KINESIS_FIELD "amazon_kinesis"
#endif
