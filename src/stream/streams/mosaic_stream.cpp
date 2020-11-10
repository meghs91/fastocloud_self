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

#include "stream/streams/mosaic_stream.h"

#include <string.h>

#include <string>

#include <common/sprintf.h>

#include "base/gst_constants.h"
#include "stream/gstreamer_utils.h"

#include "stream/elements/video/video.h"

#include "stream/streams/builders/mosaic_stream_builder.h"

#include "stream/pad/pad.h"

#define COUNT_CHUNKS 10
#define CHANNELS 2

namespace fastocloud {
namespace stream {
namespace streams {

void MosaicStream::ConnectDecodebinSignals(elements::ElementDecodebin* decodebin) {
  gboolean pad_added = decodebin->RegisterPadAddedCallback(decodebin_pad_added_callback, this);
  DCHECK(pad_added);

  gboolean autoplug_continue = decodebin->RegisterAutoplugContinue(decodebin_autoplugger_callback, this);
  DCHECK(autoplug_continue);

  gboolean autoplug_sort = decodebin->RegisterAutoplugSort(decodebin_autoplug_sort_callback, this);
  DCHECK(autoplug_sort);

  gboolean element_added = decodebin->RegisterElementAdded(decodebin_element_added_callback, this);
  DCHECK(element_added);
}

void MosaicStream::ConnectCairoSignals(elements::video::ElementCairoOverlay* cairo, const MosaicImageOptions& options) {
  options_ = options;
  gboolean cairo_draw = cairo->RegisterDrawCallback(cairo_draw_callback, this);
  DCHECK(cairo_draw);
}

gboolean MosaicStream::HandleDecodeBinAutoplugger(GstElement* elem, GstPad* pad, GstCaps* caps) {
  UNUSED(pad);

  std::string type_title;
  std::string type_full;
  if (!get_type_from_caps(caps, &type_title, &type_full)) {
    return TRUE;
  }

  element_id_t elem_id;
  const char* gst_element_name = GST_ELEMENT_NAME(elem);
  if (!GetElementId(gst_element_name, &elem_id)) {
    return TRUE;
  }
  INFO_LOG() << "Element [" << elem_id << "] caps notified: " << type_title << "(" << type_full << ")";
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
        RegisterVideoCaps(svideo, caps, elem_id);
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
        RegisterAudioCaps(saudio, caps, elem_id);
        return TRUE;
      }
      return TRUE;
    } else if (saudio == AUDIO_AAC_CODEC) {
      GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
      gint rate = 0;
      if (pad_struct && gst_structure_get_int(pad_struct, "rate", &rate)) {
        RegisterAudioCaps(saudio, caps, elem_id);
        return TRUE;
      }
      return TRUE;
    }
  }

  SupportedRawStream sraw;
  SupportedOtherType otype;
  DCHECK(IsRawStreamFromType(type_title, &sraw) || IsOtherFromType(type_title, &otype));
  return TRUE;
}

void MosaicStream::HandleDecodeBinPadAdded(GstElement* src, GstPad* new_pad) {
  const gchar* new_pad_type = pad_get_type(new_pad);
  if (!new_pad_type) {
    NOTREACHED();
    return;
  }

  INFO_LOG() << "Pad added: " << new_pad_type;
  elements::Element* dest = nullptr;
  bool is_video = strncmp(new_pad_type, "video", 5) == 0;
  bool is_audio = strncmp(new_pad_type, "audio", 5) == 0;
  bool is_subtitle = strncmp(new_pad_type, "text", 4) == 0;
  const char* gst_element_name = GST_ELEMENT_NAME(src);
  element_id_t elem_id;
  if (!GetElementId(gst_element_name, &elem_id)) {
    return;
  }

  const AudioVideoConfig* config = static_cast<const AudioVideoConfig*>(GetConfig());
  if (is_video) {
    if (config->HaveVideo() && !IsVideoInited()) {
      dest = GetElementByName(common::MemSPrintf(UDB_VIDEO_NAME_1U, elem_id));
    }
  } else if (is_audio) {
    if (config->HaveAudio() && !IsAudioInited()) {
      dest = GetElementByName(common::MemSPrintf(UDB_AUDIO_NAME_1U, elem_id));
      GstCaps* caps = gst_pad_get_current_caps(new_pad);
      GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
      if (pad_struct) {
        gint channels = 0;
        if (gst_structure_get_int(pad_struct, "channels", &channels)) {
          for (gint i = 0; i < channels; ++i) {
            if (options_.sreams.size() > elem_id) {
              options_.sreams[elem_id].sound.channels.push_back(AudioChannelInfo());
            }
          }
        }
      }

      gst_caps_unref(caps);
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
    // SetVideoInited(true);
  } else if (is_audio) {
    // SetAudioInited(true);
  }
  delete sink_pad;
}

void MosaicStream::HandleElementAdded(GstBin* bin, GstElement* element) {
  UNUSED(bin);

  const std::string element_plugin_name = elements::Element::GetPluginName(element);
  DEBUG_LOG() << "decodebin added element: " << element_plugin_name;
}

GValueArray* MosaicStream::HandleAutoplugSort(GstElement* bin, GstPad* pad, GstCaps* caps, GValueArray* factories) {
  UNUSED(bin);
  UNUSED(pad);
  UNUSED(caps);
  UNUSED(factories);
  return nullptr;
}

void MosaicStream::HandleCairoDraw(GstElement* overlay, cairo_t* cr, guint64 timestamp, guint64 duration) {
  UNUSED(overlay);
  UNUSED(duration);
  UNUSED(timestamp);

  if (!options_.isValid()) {
    return;
  }

  int right_padding = options_.right_padding;
  int width_chunk = options_.right_padding / (2 * CHANNELS);
  int padding = width_chunk;

  for (const StreamInfo& stream : options_.sreams) {
    ImageInfo img = stream.img;
    SoundInfo sound = stream.sound;

    common::draw::Size sz = img.size;
    common::draw::Point xy = img.x_y;
    int x0 = xy.x() + sz.width() - right_padding;
    int y0 = xy.y();
    int height_chuk = sz.height() / (COUNT_CHUNKS * 2);
    int x_padding = padding;
    int y_padding = height_chuk;

    for (size_t i = 0; i < COUNT_CHUNKS * 2; i += 2) {
      for (size_t j = 0; j < CHANNELS; ++j) {
        // AudioChannelInfo chan = sound.channels[j];
        double val = 0.0;
        if (sound.channels.size() > j) {
          val = sound.channels[j].rms_dB / -10;
        }
        int pos = (COUNT_CHUNKS * 2 - i) / 2;  // backward
        cairo_rectangle(cr, (x0 + x_padding) + (width_chunk * j) + (x_padding / 2 * j),
                        (y0 + y_padding) + (height_chuk * i), width_chunk, height_chuk);
        if (pos <= val) {
          if (pos <= 5) {
            cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 1);
          } else if (pos <= 8) {
            cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
          } else {
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 1);
          }
        } else {
          cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.7);
        }
        cairo_fill(cr);
      }
    }

    // cairo_rectangle(cr, x0, y0, right_padding, sz.height);
    // cairo_set_source_rgba(cr, 0.9, 0.0, 0.1, 0.7);
    // cairo_fill(cr);
  }
}

MosaicStream::MosaicStream(const EncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : IBaseStream(config, client, stats), options_() {}

const char* MosaicStream::ClassName() const {
  return "MosaicStream";
}

void MosaicStream::OnInpudSrcPadCreated(pad::Pad* src_pad, element_id_t id, const common::uri::GURL& url) {
  LinkInputPad(src_pad->GetGstPad(), id, url);
}

void MosaicStream::OnOutputSinkPadCreated(pad::Pad* sink_pad,
                                          element_id_t id,
                                          const common::uri::GURL& url,
                                          bool need_push) {
  LinkOutputPad(sink_pad->GetGstPad(), id, url, need_push);
}

void MosaicStream::OnDecodebinCreated(elements::ElementDecodebin* decodebin) {
  ConnectDecodebinSignals(decodebin);
}

void MosaicStream::OnCairoCreated(elements::video::ElementCairoOverlay* cairo, const MosaicImageOptions& options) {
  ConnectCairoSignals(cairo, options);
}

IBaseBuilder* MosaicStream::CreateBuilder() {
  const EncodeConfig* conf = static_cast<const EncodeConfig*>(GetConfig());
  return new builders::MosaicStreamBuilder(conf, this);
}

gboolean MosaicStream::HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  GstMessageType type = GST_MESSAGE_TYPE(message);
  if (type != GST_MESSAGE_ELEMENT) {
    return IBaseStream::HandleAsyncBusMessageReceived(bus, message);
  }

  const GstStructure* s = gst_message_get_structure(message);
  const gchar* name = gst_structure_get_name(s);
  if (strcmp(name, "level") != 0) {
    return IBaseStream::HandleAsyncBusMessageReceived(bus, message);
  }

  GstObject* src = GST_MESSAGE_SRC(message);
  const char* gst_element_name = GST_ELEMENT_NAME(src);
  element_id_t elem_id;
  if (!GetElementId(gst_element_name, &elem_id)) {
    return IBaseStream::HandleAsyncBusMessageReceived(bus, message);
  }

  /* the values are packed into GValueArrays with the value per channel */
  const GValue* array_val = gst_structure_get_value(s, "rms");
  GValueArray* rms_arr = static_cast<GValueArray*>(g_value_get_boxed(array_val));

  array_val = gst_structure_get_value(s, "peak");
  GValueArray* peak_arr = static_cast<GValueArray*>(g_value_get_boxed(array_val));

  array_val = gst_structure_get_value(s, "decay");
  GValueArray* decay_arr = static_cast<GValueArray*>(g_value_get_boxed(array_val));

  for (guint i = 0; i < rms_arr->n_values; ++i) {
    if (options_.sreams.size() > elem_id) {
      const GValue* value = rms_arr->values + i;
      options_.sreams[elem_id].sound.channels[i].rms_dB = g_value_get_double(value);

      value = peak_arr->values + i;
      options_.sreams[elem_id].sound.channels[i].peak_dB = g_value_get_double(value);

      value = decay_arr->values + i;
      options_.sreams[elem_id].sound.channels[i].decay_dB = g_value_get_double(value);
    }
  }
  return IBaseStream::HandleAsyncBusMessageReceived(bus, message);
}

void MosaicStream::PreLoop() {
  const AudioVideoConfig* conf = static_cast<const AudioVideoConfig*>(GetConfig());
  input_t input = conf->GetUrl();
  if (client_) {
    client_->OnInputChanged(this, input[0]);
  }
}

void MosaicStream::PostLoop(ExitStatus status) {
  UNUSED(status);
}

void MosaicStream::decodebin_pad_added_callback(GstElement* src, GstPad* new_pad, gpointer user_data) {
  MosaicStream* stream = reinterpret_cast<MosaicStream*>(user_data);
  stream->HandleDecodeBinPadAdded(src, new_pad);
}

gboolean MosaicStream::decodebin_autoplugger_callback(GstElement* elem,
                                                      GstPad* pad,
                                                      GstCaps* caps,
                                                      gpointer user_data) {
  MosaicStream* stream = reinterpret_cast<MosaicStream*>(user_data);
  return stream->HandleDecodeBinAutoplugger(elem, pad, caps);
}

void MosaicStream::cairo_draw_callback(GstElement* overlay,
                                       cairo_t* cr,
                                       guint64 timestamp,
                                       guint64 duration,
                                       gpointer user_data) {
  MosaicStream* stream = reinterpret_cast<MosaicStream*>(user_data);
  return stream->HandleCairoDraw(overlay, cr, timestamp, duration);
}

GValueArray* MosaicStream::decodebin_autoplug_sort_callback(GstElement* bin,
                                                            GstPad* pad,
                                                            GstCaps* caps,
                                                            GValueArray* factories,
                                                            gpointer user_data) {
  MosaicStream* stream = reinterpret_cast<MosaicStream*>(user_data);
  return stream->HandleAutoplugSort(bin, pad, caps, factories);
}

void MosaicStream::decodebin_element_added_callback(GstBin* bin, GstElement* element, gpointer user_data) {
  MosaicStream* stream = reinterpret_cast<MosaicStream*>(user_data);
  return stream->HandleElementAdded(bin, element);
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
