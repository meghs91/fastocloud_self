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

#include "stream/streams/encoding/encoding_stream.h"

#include <string>

#include <common/sprintf.h>

#include "base/constants.h"
#include "base/gst_constants.h"

#include "stream/elements/parser/audio.h"
#include "stream/elements/parser/video.h"
#include "stream/gstreamer_utils.h"
#include "stream/pad/pad.h"
#include "stream/streams/builders/encoding/encoding_stream_builder.h"

#if defined(MACHINE_LEARNING)
#include <fastoml/gst/gstmlmeta.h>
#include "stream/elements/machine_learning/video_ml_filter.h"
#endif

namespace fastocloud {
namespace stream {
namespace streams {

IBaseBuilder* EncodingStream::CreateBuilder() {
  const EncodeConfig* econf = static_cast<const EncodeConfig*>(GetConfig());
  return new builders::EncodingStreamBuilder(econf, this);
}

EncodingStream::EncodingStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : base_class(config, client, stats) {}

const char* EncodingStream::ClassName() const {
  return GetType() == fastotv::ENCODE ? "EncodingStream" : "CodEncodeStream";
}

void EncodingStream::HandleBufferingMessage(GstMessage* message) {
  if (IsLive()) {
    return;
  }

  gint percent;
  gst_message_parse_buffering(message, &percent);
  if (percent == 100) {
    Play();
  } else {
    Pause();
  }
  SrcDecodeBinStream::HandleBufferingMessage(message);
}

GValueArray* EncodingStream::HandleAutoplugSort(GstElement* bin, GstPad* pad, GstCaps* caps, GValueArray* factories) {
  UNUSED(bin);
  UNUSED(pad);
  UNUSED(caps);
  UNUSED(factories);
  return nullptr;
}

GstAutoplugSelectResult EncodingStream::HandleAutoplugSelect(GstElement* bin,
                                                             GstPad* pad,
                                                             GstCaps* caps,
                                                             GstElementFactory* factory) {
  UNUSED(bin);
  UNUSED(pad);
  UNUSED(factory);

  std::string type_title;
  std::string type_full;
  if (!get_type_from_caps(caps, &type_title, &type_full)) {
    return GST_AUTOPLUG_SELECT_TRY;
  }

  return GST_AUTOPLUG_SELECT_TRY;
}

gboolean EncodingStream::HandleDecodeBinAutoplugger(GstElement* elem, GstPad* pad, GstCaps* caps) {
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
        return TRUE;
      }
      return TRUE;
    } else if (saudio == AUDIO_AC3_CODEC) {
      GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
      gint rate = 0;
      if (pad_struct && gst_structure_get_int(pad_struct, "rate", &rate)) {
        RegisterAudioCaps(saudio, caps, 0);
        return TRUE;
      }
      return TRUE;
    }
    DNOTREACHED();
  }

  SupportedRawStream sraw;
  SupportedOtherType otype;
  DCHECK(IsRawStreamFromType(type_title, &sraw) || IsOtherFromType(type_title, &otype)) << "type_title: " << type_title;
  return TRUE;
}

void EncodingStream::HandleDecodeBinPadAdded(GstElement* src, GstPad* new_pad) {
  const gchar* new_pad_type = pad_get_type(new_pad);
  if (!new_pad_type) {
    NOTREACHED();
    return;
  }

  const EncodeConfig* config = static_cast<const EncodeConfig*>(GetConfig());
  INFO_LOG() << "Pad added: " << new_pad_type;
  elements::Element* dest = nullptr;
  bool is_video = strncmp(new_pad_type, "video", 5) == 0;
  bool is_audio = strncmp(new_pad_type, "audio", 5) == 0;
  bool is_subtitle = strncmp(new_pad_type, "text", 4) == 0;
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

void EncodingStream::HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) {
  UNUSED(bin);

  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  DEBUG_LOG() << "decodebin added element: " << element_plugin_name;
}

void EncodingStream::HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) {
  UNUSED(bin);

  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  DEBUG_LOG() << "decodebin removed element: " << element_plugin_name;
}

#if defined(MACHINE_LEARNING)
void EncodingStream::OnMLElementCreated(elements::machine_learning::ElementVideoMLFilter* machine) {
  ignore_result(machine->RegisterNewPredictionCallback(&EncodingStream::new_prediction_callback, this));
}

void EncodingStream::HandleMlNotification(const std::vector<fastotv::commands_info::ml::ImageBox>& images) {
  if (client_) {
    client_->OnMlNotification(this, images);
  }
}

void EncodingStream::new_prediction_callback(GstElement* elem, gpointer meta, gpointer user_data) {
  UNUSED(elem);
  UNUSED(user_data);

  std::vector<fastotv::commands_info::ml::ImageBox> images;
  GstDetectionMeta* detection_meta = static_cast<GstDetectionMeta*>(meta);
  for (int i = 0; i < detection_meta->num_boxes; ++i) {
    BBox* box = (detection_meta->boxes) + i;
    fastotv::commands_info::ml::ImageBox image;
    image.class_id = box->label;
    image.confidence = box->prob;
    image.rect.set_x(box->x);
    image.rect.set_y(box->y);
    image.rect.set_width(box->width);
    image.rect.set_height(box->height);
    images.push_back(image);
  }
  EncodingStream* stream = reinterpret_cast<EncodingStream*>(user_data);
  stream->HandleMlNotification(images);
}
#endif

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
