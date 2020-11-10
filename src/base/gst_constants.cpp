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

#include "base/gst_constants.h"

const std::array<const char*, SUPPORTED_VIDEO_PARSERS_COUNT> kSupportedVideoParsers = {
    {TS_PARSE, H264_PARSE, H265_PARSE}};

const std::array<const char*, SUPPORTED_AUDIO_PARSERS_COUNT> kSupportedAudioParsers = {
    {MPEG_AUDIO_PARSE, AAC_PARSE, AC3_PARSE, RAW_AUDIO_PARSE}};

const std::array<const char*, SUPPORTED_VIDEO_ENCODERS_COUNT> kSupportedVideoEncoders = {
    {EAVC_ENC, OPEN_H264_ENC, X264_ENC, NV_H264_ENC, NV_H265_ENC, VAAPI_H264_ENC, VAAPI_MPEG2_ENC, MFX_H264_ENC,
     X265_ENC, MSDK_H264_ENC}};
const std::array<const char*, SUPPORTED_AUDIO_ENCODERS_COUNT> kSupportedAudioEncoders = {
    {LAME_MP3_ENC, FAAC, VOAAC_ENC}};
