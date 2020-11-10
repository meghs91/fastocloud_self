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

#include "stream/stypes.h"

#include <regex>

#include <common/convert2string.h>
#include <common/sprintf.h>

#define APPLICATION_HLS "application/x-hls"
#define APPLICATION_ICY "application/x-icy"
#define APPLICATION_TELETEXT "application/x-teletext"
#define APPLICATION_GZIP "application/x-gzip"
#define APPLICATION_XRTP "application/x-rtp"

#define IMAGE_PNG "image/png"
#define IMAGE_JPEG "image/jpeg"
#define SUBPICTURE_DVB "subpicture/x-dvb"

#define DEMUXER_FLV "video/x-flv"
#define DEMUXER_MPEGTS "video/mpegts"

#define VIDEO_MPEG "video/mpeg"
#define VIDEO_H264 "video/x-h264"
#define VIDEO_H265 "video/x-h265"

#define AUDIO_MPEG "audio/mpeg"
#define AUDIO_AC3 "audio/x-ac3"

#define RAW_VIDEO "video/x-raw"
#define RAW_AUDIO "audio/x-raw"

namespace fastocloud {
namespace stream {

bool IsScreenUrl(const common::uri::GURL& url) {
  return url == common::uri::GURL(SCREEN_URL);
}

bool IsDecklinkUrl(const common::uri::GURL& url) {
  return url == common::uri::GURL(DECKLINK_URL);
}

bool IsDeviceOutUrl(const common::uri::GURL& url, SinkDeviceType* type) {
  if (IsScreenUrl(url)) {
    if (type) {
      *type = SCREEN_OUTPUT;
    }
    return true;
  } else if (IsDecklinkUrl(url)) {
    if (type) {
      *type = DECKLINK_OUTPUT;
    }
    return true;
  }

  return false;
}

bool IsRecordingUrl(const common::uri::GURL& url) {
  return url == common::uri::GURL(RECORDING_URL);
}

bool IsFakeUrl(const common::uri::GURL& url) {
  return url == common::uri::GURL(FAKE_URL);
}

bool GetElementId(const std::string& name, element_id_t* elem_id) {
  if (!elem_id) {
    return false;
  }

  static const std::regex digit_end("^([^_]+_)+([0-9]+)$");
  std::smatch match_results;
  if (!std::regex_match(name, match_results, digit_end)) {
    return false;
  }

  element_id_t lid;
  if (common::ConvertFromString(match_results.str(2), &lid)) {
    *elem_id = lid;
  }
  return true;
}

bool GetPadId(const std::string& name, int* pad_id) {
  if (!pad_id) {
    return false;
  }

  static const std::regex digit_end("^([^_]+_)+([0-9]+)$");
  std::smatch match_results;
  if (!std::regex_match(name, match_results, digit_end)) {
    return false;
  }

  int lpad_id;
  if (common::ConvertFromString(match_results.str(2), &lpad_id)) {
    *pad_id = lpad_id;
  }
  return true;
}

bool IsOtherFromType(const std::string& type, SupportedOtherType* oc) {
  if (type.empty() || !oc) {
    return false;
  }

  if (type == APPLICATION_HLS) {
    *oc = APPLICATION_HLS_TYPE;
    return true;
  } else if (type == APPLICATION_ICY) {
    *oc = APPLICATION_ICY_TYPE;
    return true;
  } else if (type == APPLICATION_TELETEXT) {
    *oc = APPLICATION_TELETEXT_TYPE;
    return true;
  } else if (type == APPLICATION_GZIP) {
    *oc = APPLICATION_GZIP_TYPE;
    return true;
  } else if (type == APPLICATION_XRTP) {
    *oc = APPLICATION_XRTP_TYPE;
    return true;
  } else if (type == IMAGE_PNG) {
    *oc = IMAGE_PNG_TYPE;
    return true;
  } else if (type == IMAGE_JPEG) {
    *oc = IMAGE_JPEG_TYPE;
    return true;
  } else if (type == SUBPICTURE_DVB) {
    *oc = SUBPICTURE_DVB_TYPE;
    return true;
  }

  return false;
}

bool IsDemuxerFromType(const std::string& type, SupportedDemuxer* dc) {
  if (type.empty() || !dc) {
    return false;
  }

  if (type == DEMUXER_MPEGTS) {
    *dc = VIDEO_MPEGTS_DEMUXER;
    return true;
  } else if (type == DEMUXER_FLV) {
    *dc = VIDEO_FLV_DEMUXER;
    return true;
  }

  return false;
}

bool IsVideoCodecFromType(const std::string& type, SupportedVideoCodec* vc) {
  if (type.empty() || !vc) {
    return false;
  }

  if (type == VIDEO_MPEG) {
    *vc = VIDEO_MPEG_CODEC;
    return true;
  } else if (type == VIDEO_H264) {
    *vc = VIDEO_H264_CODEC;
    return true;
  } else if (type == VIDEO_H265) {
    *vc = VIDEO_H265_CODEC;
    return true;
  }

  return false;
}

bool IsAudioCodecFromType(const std::string& type, SupportedAudioCodec* ac) {
  if (type.empty() || !ac) {
    return false;
  }

  if (type == AUDIO_MPEG) {
    *ac = AUDIO_MPEG_CODEC;
    return true;
  } else if (type == AUDIO_AC3) {
    *ac = AUDIO_AC3_CODEC;
    return true;
  }

  return false;
}

bool IsRawStreamFromType(const std::string& type, SupportedRawStream* rc) {
  if (type.empty() || !rc) {
    return false;
  }

  if (type == RAW_VIDEO) {
    *rc = VIDEO_RAW_STREAM;
    return true;
  } else if (type == RAW_AUDIO) {
    *rc = AUDIO_RAW_STREAM;
    return true;
  }

  return false;
}

std::string GenHttpTsTemplate(fastotv::timestamp_t msec) {
  return common::MemSPrintf("%llu_%s", msec, TS_TEMPLATE);
}

std::string GenVodHttpTsTemplate() {
  return TS_TEMPLATE;
}

bool GetIndexFromHttpTsTemplate(const std::string& file_name, uint64_t* index) {
  if (!index) {
    return false;
  }

  std::smatch match;
  static const std::regex chunk_index("^[0-9]+_([0-9]+)" CHUNK_EXT "$");
  if (std::regex_match(file_name, match, chunk_index)) {
    uint64_t t;
    if (common::ConvertFromString(match.str(1), &t)) {
      *index = t;
      return true;
    }
  }
  return false;
}

}  // namespace stream
}  // namespace fastocloud
