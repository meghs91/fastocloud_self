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

#include "stream/elements/element.h"

#include <gst/gstelement.h>  // for GstElement

#include "stream/gstreamer_utils.h"  // for make_element_safe

#include "base/gst_constants.h"  // for AAC_PARSE, AC3_PARSE, ALSA_SRC

#include "stream/pad/pad.h"
#include "stream/stypes.h"

#define DECLARE_ELEMENT_TRAITS_SPECIALIZATION(x)    \
  template <>                                       \
  const char* ElementsTraits<ELEMENT_##x>::name() { \
    return x;                                       \
  }

namespace fastocloud {
namespace stream {
namespace elements {

std::string Element::GetPluginName() const {
  return plugin_name_;
}

std::string Element::GetName() const {
  return name_;
}

bool Element::IsValid() const {
  return element_ != nullptr;
}

GstElement* Element::GetGstElement() const {
  return element_;
}

pad::Pad* Element::StaticPad(const gchar* name) const {
  GstPad* pad = gst_element_get_static_pad(element_, name);
  pad::Pad* p = new pad::Pad(pad);
  if (pad) {
    gst_object_unref(pad);
  }
  return p;
}

pad::Pad* Element::RequestPad(const gchar* name) const {
  GstPad* pad = gst_element_get_request_pad(element_, name);
  pad::Pad* p = new pad::Pad(pad);
  if (pad) {
    gst_object_unref(pad);
  }
  return p;
}

void Element::ReleaseRequestedPad(pad::Pad* pad) {
  if (!pad || !pad->IsValid()) {
    DNOTREACHED();
    return;
  }

  gst_element_release_request_pad(GetGstElement(), pad->GetGstPad());
}

Element::Element(const std::string& plugin_name, const std::string& name)
    : Element(plugin_name, name, make_element_safe(plugin_name, name)) {
  element_id_t elem_id;
  if (!GetElementId(name, &elem_id)) {
    WARNING_LOG() << "Not template plugin '" << plugin_name << "' named: " << name;
  }
}

Element::Element(const std::string& plugin_name, const std::string& name, GstElement* const element)
    : name_(name), plugin_name_(plugin_name), element_(element) {}

Element::~Element() {
  for (size_t i = 0; i < signals_.size(); ++i) {
    UnRegisterCallback(signals_[i]);
  }

  signals_.clear();
}

gboolean Element::RegisterCallback(const char* signal_name, GCallback cb, gpointer user_data) {
  gulong id = g_signal_connect(element_, signal_name, cb, user_data);
  if (id == 0) {
    return FALSE;
  }

  signals_.push_back(id);
  return TRUE;
}

gboolean Element::RegisterPadAddedCallback(pad_added_callback_t cb, gpointer user_data) {
  return RegisterCallback("pad-added", G_CALLBACK(cb), user_data);
}

gboolean Element::RegisterPadRemovedCallback(pad_removed_callback_t cb, gpointer user_data) {
  return RegisterCallback("pad-removed", G_CALLBACK(cb), user_data);
}

gboolean Element::RegisterNoMorePadsCallback(no_more_pads_callback_t cb, gpointer user_data) {
  return RegisterCallback("no-more-pads", G_CALLBACK(cb), user_data);
}

void Element::UnRegisterCallback(gulong signal_id) {
  g_signal_handler_disconnect(element_, signal_id);
}

void Element::SetProperty(const char* property, bool val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, gfloat val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, gdouble val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, gint8 val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, guint8 val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, gint16 val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, guint16 val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, gint val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, guint val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, gint64 val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, guint64 val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, const char* val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetProperty(const char* property, const std::string& val) {
  SetProperty(property, val.c_str());
}

void Element::SetProperty(const char* property, void* val) {
  g_object_set(element_, property, val, nullptr);
}

void Element::SetFractionProperty(const char* property, gint num, gint den) {
  g_object_set(element_, property, num, den, nullptr);
}

GValue Element::GetProperty(const char* property, GType type) const {
  GValue value = G_VALUE_INIT;
  g_value_init(&value, type);
  g_object_get_property(G_OBJECT(element_), property, &value);
  return value;
}

std::string Element::GetStringProperty(const char* property) const {
  GValue gvalue = GetProperty(property, G_TYPE_STRING);
  return gvalue_cast<const char*>(&gvalue);
}

std::string Element::GetElementName(GstElement* element) {
  if (!element) {
    return std::string();
  }

  gchar* name = gst_element_get_name(element);
  if (!name) {
    return std::string();
  }
  std::string name_str(name);
  g_free(name);

  return name_str;
}

std::string Element::GetPluginName(GstElement* element) {
  if (!element) {
    return std::string();
  }

  GstElementFactory* factory = gst_element_get_factory(element);
  gpointer plug_feature = GST_PLUGIN_FEATURE(factory);
  if (!plug_feature) {
    return std::string();
  }

  return gst_plugin_feature_get_name(plug_feature);
}

void ElementDecodebin::SetUseBuffering(bool use_buffering) {
  SetProperty("use-buffering", use_buffering);
}

void ElementDecodebin::SetMaxSizeBuffers(guint val) {
  SetProperty("max-size-buffers", val);
}

void ElementDecodebin::SetMaxSizeTime(guint val) {
  SetProperty("max-size-time", val);
}

void ElementDecodebin::SetMaxSizeBytes(guint64 val) {
  SetProperty("max-size-bytes", val);
}

void ElementDecodebin::SetEmpty() {
  SetMaxSizeBuffers(0);
  SetMaxSizeTime(0);
  SetMaxSizeBytes(0);
}

gboolean ElementDecodebin::RegisterAutoplugContinue(autoplug_continue_callback_t cb, gpointer user_data) {
  return RegisterCallback("autoplug-continue", G_CALLBACK(cb), user_data);
}

gboolean ElementDecodebin::RegisterAutoplugSelect(autoplug_select_callback_t cb, gpointer user_data) {
  return RegisterCallback("autoplug-select", G_CALLBACK(cb), user_data);
}

gboolean ElementDecodebin::RegisterAutoplugSort(autoplug_sort_callback_t cb, gpointer user_data) {
  return RegisterCallback("autoplug-sort", G_CALLBACK(cb), user_data);
}

void ElementQueue::SetMaxSizeBuffers(guint val) {
  SetProperty("max-size-buffers", val);
}

void ElementQueue::SetMaxSizeTime(guint val) {
  SetProperty("max-size-time", val);
}

void ElementQueue::SetMaxSizeBytes(guint64 val) {
  SetProperty("max-size-bytes", val);
}

void ElementQueue::SetEmpty() {
  SetMaxSizeBuffers(0);
  SetMaxSizeTime(0);
  SetMaxSizeBytes(0);
}

void ElementCapsFilter::SetCaps(GstCaps* caps) {
  SetProperty("caps", caps);
}

void ElementQueue2::SetMaxSizeBuffers(guint val) {
  SetProperty("max-size-buffers", val);
}

void ElementQueue2::SetMaxSizeTime(guint64 val) {
  SetProperty("max-size-time", val);
}

void ElementQueue2::SetMaxSizeBytes(guint val) {
  SetProperty("max-size-bytes", val);
}

void ElementQueue2::SetUseBuffering(bool use_buffering) {
  SetProperty("use-buffering", use_buffering);
}

void ElementAvdecH264::SetMaxThreads(gint threads) {
  SetProperty("max-threads", threads);
}

void ElementAvdecH264::SetSkipFrame(gint skip_frame) {
  SetProperty("skip-frame", skip_frame);
}

void ElementAvdecH264::SetOutputCorrupt(gboolean output_corrupt) {
  SetProperty("output-corrupt", output_corrupt);
}

void ElementVaapiPostProc::SetDinterlaceMode(gint deinterlace_method) {
  SetProperty("deinterlace-method", deinterlace_method);
}

void ElementVaapiPostProc::SetFormat(gint format) {
  SetProperty("format", format);
}

void ElementVaapiPostProc::SetWidth(guint width) {
  SetProperty("width", width);
}

void ElementVaapiPostProc::SetHeight(guint height) {
  SetProperty("height", height);
}

void ElementVaapiPostProc::SetForceAspectRatio(bool ratio) {
  SetProperty("force-aspect-ratio", ratio);
}

void ElementVaapiPostProc::SetScaleMethod(guint met) {
  SetProperty("scale-method", met);
}

void ElementVaapiPostProc::SetDeinerlaceMethod(guint met) {
  SetProperty("deinterlace-method", met);
}

void ElementVaapiPostProc::SetDenoise(gfloat denoise) {
  SetProperty("denoise", denoise);
}

void ElementVaapiPostProc::SetSkinToneEnhancement(bool val) {
  SetProperty("skin-tone-enhancement", val);
}

void ElementMFXVpp::SetForceAspectRatio(bool ratio) {
  SetProperty("force-aspect-ratio", ratio);
}

void ElementMFXVpp::SetWidth(gint width) {
  SetProperty("width", width);
}

void ElementMFXVpp::SetHeight(gint height) {
  SetProperty("height", height);
}

void ElementMFXVpp::SetFrameRate(int framerate) {
  SetFractionProperty("framerate", framerate, 1);
}

void ElementMFXVpp::SetDinterlaceMode(int mode) {
  SetProperty("deinterlace-mode", mode);
}

void ElementMFXH264Decode::SetLiveMode(bool mode) {
  SetProperty("live-mode", mode);
}

void ElementTsDemux::SetParsePrivateSections(gboolean parse_private_sections) {
  SetProperty("parse-private-sections", parse_private_sections);
}

void ElementTsDemux::SetProgramNumber(gint number) {
  SetProperty("program-number", number);
}

DECLARE_ELEMENT_TRAITS_SPECIALIZATION(DECODEBIN)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(FAKE_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TEST_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_TEST_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUDIO_TEST_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(DISPLAY_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_SCREEN_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUDIO_SCREEN_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(QUEUE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(QUEUE2)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(H264_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(H265_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MPEG_VIDEO_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AAC_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AC3_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MPEG_AUDIO_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RAW_AUDIO_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TEE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(FLV_MUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MP4_MUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(QT_MUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MPEGTS_MUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(FILE_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MULTIFILE_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_MUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_MPEG2_PAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_H264_PAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_H265_PAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_AAC_PAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_AC3_PAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_MPEG2_DEPAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_H264_DEPAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_H265_DEPAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_AAC_DEPAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTP_AC3_DEPAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(V4L2_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(SPLIT_MUX_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(ALSA_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MULTIFILE_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(APP_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(FILE_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(IMAGE_FREEZE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(CAPS_FILTER)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUDIO_CONVERT)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RG_VOLUME)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VOLUME)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(FAAC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VOAAC_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUDIO_RESAMPLE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(LAME_MP3_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_CONVERT)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(DEINTERLACE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(ASPECT_RATIO)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AV_DEINTERLACE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(UDP_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TCP_SERVER_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTMP_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(HLS_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(KVS_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(SOUP_HTTP_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(DVB_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_SCALE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_RATE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(NV_H264_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(NV_H265_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MSDK_H264_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(X264_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(X265_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MPEG2_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(EAVC_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(OPEN_H264_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(UDP_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TCP_SERVER_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTMP_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RTSP_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VAAPI_H264_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VAAPI_MPEG2_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MFX_H264_ENC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(GDK_PIXBUF_OVERLAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(RSVG_OVERLAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_BOX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_MIXER)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUDIO_MIXER)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(INTERLEAVE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(DEINTERLEAVE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(CAIRO_OVERLAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TEXT_OVERLAY)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_CROP)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(SPECTRUM)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(LEVEL)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(HLS_DEMUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VIDEO_DECK_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUDIO_DECK_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(INTERLACE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AUTO_VIDEO_CONVERT)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TS_PARSE)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AVDEC_H264)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TS_DEMUX)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AVDEC_AC3)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(AVDEC_AC3_FIXED)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(SOUP_HTTP_CLIENT_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VAAPI_DECODEBIN)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(VAAPI_POST_PROC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MFX_VPP)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(MFX_H264_DEC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(SRT_SRC)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(SRT_SINK)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TINY_YOLOV2)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(TINY_YOLOV3)
DECLARE_ELEMENT_TRAITS_SPECIALIZATION(DETECTION_OVERLAY)

}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
