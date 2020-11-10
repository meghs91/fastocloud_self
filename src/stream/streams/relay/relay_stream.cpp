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

#include "stream/streams/relay/relay_stream.h"

#include <string.h>

#include <string>

#include <common/sprintf.h>

#include "base/constants.h"
#include "base/gst_constants.h"

#include "stream/elements/parser/video.h"
#include "stream/gstreamer_utils.h"  // for pad_get_type
#include "stream/pad/pad.h"          // for Pad
#include "stream/streams/builders/relay/relay_stream_builder.h"

namespace {
const char kAvdecMpeg2Video[] = "avdec_mpeg2video";
const char kAvdecMpegVideo[] = "avdec_mpeg2video";
}  // namespace

namespace fastocloud {
namespace stream {
namespace streams {

RelayStream::RelayStream(const RelayConfig* config, IStreamClient* client, StreamStruct* stats)
    : SrcDecodeBinStream(config, client, stats) {}

const char* RelayStream::ClassName() const {
  return "RelayStream";
}

IBaseBuilder* RelayStream::CreateBuilder() {
  const RelayConfig* rconf = static_cast<const RelayConfig*>(GetConfig());
  return new builders::RelayStreamBuilder(rconf, this);
}

gboolean RelayStream::HandleDecodeBinAutoplugger(GstElement* elem,
                                                 GstPad* pad,
                                                 GstCaps* caps) {  // FALSE => stop autoplug
  UNUSED(elem);
  UNUSED(pad);

  std::string type_title;
  std::string type_full;
  if (!get_type_from_caps(caps, &type_title, &type_full)) {
    WARNING_LOG() << "Can't parse caps";
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
        return FALSE;
      }
      return TRUE;
    } else if (svideo == VIDEO_H265_CODEC) {
      return FALSE;
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

  return TRUE;
}

void RelayStream::HandleDecodeBinPadAdded(GstElement* src, GstPad* new_pad) {
  const gchar* new_pad_type = pad_get_type(new_pad);
  if (!new_pad_type) {
    DNOTREACHED();
    return;
  }

  const RelayConfig* config = static_cast<const RelayConfig*>(GetConfig());
  bool is_video = strncmp(new_pad_type, "video", 5) == 0;
  bool is_audio = strncmp(new_pad_type, "audio", 5) == 0;
  bool is_subtitle = strncmp(new_pad_type, "text", 4) == 0;
  elements::Element* dest = nullptr;
  if (is_video) {
    if (config->HaveVideo() && !IsVideoInited()) {
      dest = GetElementByName(common::MemSPrintf(UDB_VIDEO_NAME_1U, 0));
    }
  } else if (is_audio) {
    if (config->HaveAudio() && !IsAudioInited()) {
      const char* gst_pad_name = GST_PAD_NAME(new_pad);
      const auto audio_select = config->GetAudioSelect();
      int current_audio_track = 0;
      if (!audio_select || (GetPadId(gst_pad_name, &current_audio_track) && *audio_select == current_audio_track)) {
        dest = GetElementByName(common::MemSPrintf(UDB_AUDIO_NAME_1U, 0));
      }
    }
  } else if (is_subtitle) {
    if (config->HaveSubtitle()) {
    }
  } else {
    // something else
  }

  if (!dest) {
    return;
  }

  pad::Pad* sink_pad = dest->StaticPad("sink");
  if (!sink_pad->IsValid()) {
    return;
  }

  if (!gst_pad_is_linked(sink_pad->GetGstPad())) {
    GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad->GetGstPad());
    if (GST_PAD_LINK_FAILED(ret)) {
      WARNING_LOG() << "Failed to link: " << GST_ELEMENT_NAME(src) << " " << GST_PAD_NAME(new_pad) << " "
                    << new_pad_type;
    } else {
      DEBUG_LOG() << "Pad emitted: " << GST_ELEMENT_NAME(src) << " " << GST_PAD_NAME(new_pad) << " " << new_pad_type;
    }
  } else {
    DEBUG_LOG() << "pad-emitter: pad is linked";
  }

  if (is_video) {
    SetVideoInited(true);
  } else if (is_audio) {
    SetAudioInited(true);
  }
  delete sink_pad;
}

GValueArray* RelayStream::HandleAutoplugSort(GstElement* bin, GstPad* pad, GstCaps* caps, GValueArray* factories) {
  UNUSED(bin);
  UNUSED(pad);
  UNUSED(caps);
  UNUSED(factories);
  return nullptr;
}

GstAutoplugSelectResult RelayStream::HandleAutoplugSelect(GstElement* bin,
                                                          GstPad* pad,
                                                          GstCaps* caps,
                                                          GstElementFactory* factory) {
  UNUSED(bin);
  UNUSED(pad);

  std::string type_title;
  std::string type_full;
  if (!get_type_from_caps(caps, &type_title, &type_full)) {
    return GST_AUTOPLUG_SELECT_TRY;
  }

  gpointer plug_feature = GST_PLUGIN_FEATURE(factory);
  const gchar* factory_name = gst_plugin_feature_get_name(plug_feature);

  if (factory_name == kAvdecMpegVideo || factory_name == kAvdecMpeg2Video) {
    INFO_LOG() << "skip: " << factory_name;
    return GST_AUTOPLUG_SELECT_SKIP;
  }

  return GST_AUTOPLUG_SELECT_TRY;
}

void RelayStream::HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  DEBUG_LOG() << "decodebin added element: " << element_plugin_name;
}

void RelayStream::HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  DEBUG_LOG() << "decodebin removed element: " << element_plugin_name;
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
