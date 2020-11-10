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

#include "stream/streams/encoding/encoding_only_video_stream.h"

#include <string>

#include "stream/gstreamer_utils.h"

#include "stream/streams/builders/encoding/encoding_only_video_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {

EncodingOnlyVideoStream::EncodingOnlyVideoStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : EncodingStream(config, client, stats) {}

const char* EncodingOnlyVideoStream::ClassName() const {
  return "EncodingOnlyVideoStream";
}

IBaseBuilder* EncodingOnlyVideoStream::CreateBuilder() {
  const EncodeConfig* econf = static_cast<const EncodeConfig*>(GetConfig());
  return new builders::EncodingOnlyVideoStreamBuilder(econf, this);
}

gboolean EncodingOnlyVideoStream::HandleDecodeBinAutoplugger(GstElement* elem, GstPad* pad, GstCaps* caps) {
  UNUSED(elem);
  UNUSED(pad);

  std::string type_title;
  std::string type_full;
  if (!get_type_from_caps(caps, &type_title, &type_full)) {
    return TRUE;
  }

  INFO_LOG() << "Caps notified: " << type_title << "(" << type_full << ")";
  SupportedAudioCodec saudio;
  SupportedVideoCodec svideo;
  SupportedDemuxer sdemuxer;
  bool is_audio = IsAudioCodecFromType(type_title, &saudio);
  bool is_video = IsVideoCodecFromType(type_title, &svideo);
  bool is_demuxer = IsDemuxerFromType(type_title, &sdemuxer);
  if (is_demuxer) {
    if (sdemuxer == VIDEO_MPEGTS_DEMUXER) {
      return TRUE;
    } else if (sdemuxer == VIDEO_FLV_DEMUXER) {
      return TRUE;
    }
  } else if (is_video) {
    if (svideo == VIDEO_H264_CODEC) {
      GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
      gint width = 0;
      gint height = 0;
      if (pad_struct && gst_structure_get_int(pad_struct, "width", &width) &&
          gst_structure_get_int(pad_struct, "height", &height)) {
        RegisterVideoCaps(svideo, caps, 0);
        return TRUE;
      }
      return TRUE;
    } else if (svideo == VIDEO_H265_CODEC) {
      return TRUE;
    } else if (svideo == VIDEO_MPEG_CODEC) {
      return TRUE;
    }
    DNOTREACHED();
  } else if (is_audio) {
    if (saudio == AUDIO_MPEG_CODEC) {
      GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
      gint rate = 0;
      if (pad_struct && gst_structure_get_int(pad_struct, "rate", &rate)) {
        RegisterAudioCaps(saudio, caps, 0);
        return FALSE;
      }
      return TRUE;
    } else if (saudio == AUDIO_AC3_CODEC) {
      GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
      gint rate = 0;
      if (pad_struct && gst_structure_get_int(pad_struct, "rate", &rate)) {
        RegisterAudioCaps(saudio, caps, 0);
        return FALSE;
      }
      return TRUE;
    }
    DNOTREACHED();
  }

  SupportedRawStream sraw;
  SupportedOtherType otype;
  DCHECK(IsRawStreamFromType(type_title, &sraw) || IsOtherFromType(type_title, &otype));
  return TRUE;
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
