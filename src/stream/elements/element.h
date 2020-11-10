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

#include <string>  // for string
#include <vector>

#include <gst/gstbin.h>
#include <gst/gstobject.h>

#include <common/macros.h>

enum GstAutoplugSelectResult { GST_AUTOPLUG_SELECT_TRY, GST_AUTOPLUG_SELECT_EXPOSE, GST_AUTOPLUG_SELECT_SKIP };

namespace fastocloud {
namespace stream {
namespace pad {
class Pad;
}
namespace elements {

enum SupportedElements {
  ELEMENT_DECODEBIN,
  ELEMENT_FAKE_SINK,
  ELEMENT_TEST_SINK,
  ELEMENT_VIDEO_TEST_SRC,
  ELEMENT_AUDIO_TEST_SRC,
  ELEMENT_DISPLAY_SRC,
  ELEMENT_VIDEO_SCREEN_SINK,
  ELEMENT_AUDIO_SCREEN_SINK,
  ELEMENT_QUEUE,
  ELEMENT_QUEUE2,
  ELEMENT_H264_PARSE,
  ELEMENT_H265_PARSE,
  ELEMENT_MPEG_VIDEO_PARSE,
  ELEMENT_AAC_PARSE,
  ELEMENT_AC3_PARSE,
  ELEMENT_MPEG_AUDIO_PARSE,
  ELEMENT_RAW_AUDIO_PARSE,
  ELEMENT_TEE,
  ELEMENT_FLV_MUX,
  ELEMENT_MP4_MUX,
  ELEMENT_QT_MUX,
  ELEMENT_MPEGTS_MUX,
  ELEMENT_FILE_SINK,
  ELEMENT_RTP_MUX,
  ELEMENT_RTP_MPEG2_PAY,
  ELEMENT_RTP_H264_PAY,
  ELEMENT_RTP_H265_PAY,
  ELEMENT_RTP_AAC_PAY,
  ELEMENT_RTP_AC3_PAY,
  ELEMENT_RTP_MPEG2_DEPAY,
  ELEMENT_RTP_H264_DEPAY,
  ELEMENT_RTP_H265_DEPAY,
  ELEMENT_RTP_AAC_DEPAY,
  ELEMENT_RTP_AC3_DEPAY,
  ELEMENT_V4L2_SRC,
  ELEMENT_SPLIT_MUX_SINK,
  ELEMENT_ALSA_SRC,
  ELEMENT_MULTIFILE_SRC,
  ELEMENT_APP_SRC,
  ELEMENT_FILE_SRC,
  ELEMENT_IMAGE_FREEZE,
  ELEMENT_CAPS_FILTER,
  ELEMENT_AUDIO_CONVERT,
  ELEMENT_RG_VOLUME,
  ELEMENT_VOLUME,
  ELEMENT_FAAC,
  ELEMENT_VOAAC_ENC,
  ELEMENT_AUDIO_RESAMPLE,
  ELEMENT_LAME_MP3_ENC,
  ELEMENT_VIDEO_CONVERT,
  ELEMENT_ASPECT_RATIO,
  ELEMENT_DEINTERLACE,
  ELEMENT_AV_DEINTERLACE,
  ELEMENT_UDP_SINK,
  ELEMENT_TCP_SERVER_SINK,
  ELEMENT_RTMP_SINK,
  ELEMENT_HLS_SINK,
  ELEMENT_MULTIFILE_SINK,
  ELEMENT_KVS_SINK,
  ELEMENT_SOUP_HTTP_SRC,
  ELEMENT_DVB_SRC,
  ELEMENT_VIDEO_SCALE,
  ELEMENT_VIDEO_RATE,
  ELEMENT_NV_H264_ENC,
  ELEMENT_NV_H265_ENC,
  ELEMENT_MSDK_H264_ENC,
  ELEMENT_X264_ENC,
  ELEMENT_X265_ENC,
  ELEMENT_MPEG2_ENC,
  ELEMENT_EAVC_ENC,
  ELEMENT_OPEN_H264_ENC,
  ELEMENT_UDP_SRC,
  ELEMENT_TCP_SERVER_SRC,
  ELEMENT_RTMP_SRC,
  ELEMENT_RTSP_SRC,
  ELEMENT_VAAPI_H264_ENC,
  ELEMENT_VAAPI_MPEG2_ENC,
  ELEMENT_MFX_H264_ENC,
  ELEMENT_GDK_PIXBUF_OVERLAY,
  ELEMENT_RSVG_OVERLAY,
  ELEMENT_VIDEO_BOX,
  ELEMENT_VIDEO_MIXER,
  ELEMENT_AUDIO_MIXER,
  ELEMENT_INTERLEAVE,
  ELEMENT_DEINTERLEAVE,
  ELEMENT_CAIRO_OVERLAY,
  ELEMENT_TEXT_OVERLAY,
  ELEMENT_VIDEO_CROP,
  ELEMENT_SPECTRUM,
  ELEMENT_LEVEL,
  ELEMENT_HLS_DEMUX,
  ELEMENT_VIDEO_DECK_SINK,
  ELEMENT_AUDIO_DECK_SINK,
  ELEMENT_INTERLACE,
  ELEMENT_AUTO_VIDEO_CONVERT,
  ELEMENT_TS_PARSE,
  ELEMENT_AVDEC_H264,
  ELEMENT_TS_DEMUX,
  ELEMENT_AVDEC_AC3,
  ELEMENT_AVDEC_AC3_FIXED,
  ELEMENT_SOUP_HTTP_CLIENT_SINK,
  ELEMENT_VAAPI_DECODEBIN,
  ELEMENT_VAAPI_POST_PROC,
  ELEMENT_MFX_VPP,
  ELEMENT_MFX_H264_DEC,
  ELEMENT_SRT_SRC,
  ELEMENT_SRT_SINK,
  ELEMENT_TINY_YOLOV2,
  ELEMENT_TINY_YOLOV3,
  ELEMENT_DETECTION_OVERLAY,
  ELEMENTS_COUNT
};

template <SupportedElements el>
struct ElementsTraits {
  enum { value = el };
  static const char* name();
};

// wrapper for GstElement
class Element {
 public:
  typedef void (*pad_added_callback_t)(GstElement* self, GstPad* new_pad, gpointer user_data);
  typedef void (*pad_removed_callback_t)(GstElement* self, GstPad* old_pad, gpointer user_data);
  typedef void (*no_more_pads_callback_t)(GstElement* self, gpointer user_data);

  std::string GetPluginName() const;  // gst plugin name
  std::string GetName() const;
  bool IsValid() const;

  GstElement* GetGstElement() const;
  pad::Pad* StaticPad(const gchar* name) const;   // allocate pad::Pad
  pad::Pad* RequestPad(const gchar* name) const;  // allocate pad::Pad
  void ReleaseRequestedPad(pad::Pad* pad);

  virtual ~Element();

  void SetProperty(const char* property, bool val);
  void SetProperty(const char* property, gfloat val);
  void SetProperty(const char* property, gdouble val);
  void SetProperty(const char* property, gint8 val);
  void SetProperty(const char* property, guint8 val);
  void SetProperty(const char* property, gint16 val);
  void SetProperty(const char* property, guint16 val);
  void SetProperty(const char* property, gint val);
  void SetProperty(const char* property, guint val);
  void SetProperty(const char* property, gint64 val);
  void SetProperty(const char* property, guint64 val);
  void SetProperty(const char* property, const char* val);
  void SetProperty(const char* property, const std::string& val);
  void SetProperty(const char* property, void* val);
  void SetFractionProperty(const char* property, gint num, gint den);

  GValue GetProperty(const char* property, GType type) const;
  std::string GetStringProperty(const char* property) const;

  static std::string GetElementName(GstElement* element);
  static std::string GetPluginName(GstElement* element);

  gboolean RegisterPadAddedCallback(pad_added_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
  gboolean RegisterPadRemovedCallback(pad_removed_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
  gboolean RegisterNoMorePadsCallback(no_more_pads_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;

 protected:
  Element(const std::string& plugin_name,
          const std::string& name);  // allocate element
  Element(const std::string& plugin_name,
          const std::string& name,
          GstElement* const element);  // take poiner of element

  gboolean RegisterCallback(const char* signal_name, GCallback cb, gpointer user_data) WARN_UNUSED_RESULT;

 private:
  void UnRegisterCallback(gulong signal_id);

  const std::string name_;
  const std::string plugin_name_;
  GstElement* const element_;

  std::vector<gulong> signals_;
};

template <SupportedElements el>
class ElementEx : public Element {
 public:
  typedef ElementsTraits<el> traits_t;
  typedef Element base_class;

  explicit ElementEx(const std::string& name) : Element(GetPluginName(), name) {}  // allocate element
  ElementEx(const std::string& name, GstElement* const element)
      : Element(GetPluginName(), name, element) {}  // take poiner of element, only wrap

  static std::string GetPluginName() { return traits_t::name(); }
};

template <SupportedElements el>
class ElementBinEx : public ElementEx<el> {
 public:
  typedef ElementEx<el> base_class;
  using base_class::base_class;

  typedef void (*element_added_callback_t)(GstBin* bin, GstElement* element, gpointer user_data);
  typedef void (*element_removed_callback_t)(GstBin* bin, GstElement* element, gpointer user_data);
  typedef void (*element_deep_added_callback_t)(GstBin* bin, GstBin* sub_bin, GstElement* element, gpointer user_data);
  typedef void (*element_deep_removed_callback_t)(GstBin* bin,
                                                  GstBin* sub_bin,
                                                  GstElement* element,
                                                  gpointer user_data);

  GstElement* GetElementByName(const char* name) {
    return gst_bin_get_by_name(GST_BIN(base_class::GetGstElement()), name);
  }

  gboolean RegisterElementAdded(element_added_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT {
    return base_class::RegisterCallback("element-added", G_CALLBACK(cb), user_data);
  }

  gboolean RegisterElementRemoved(element_removed_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT {
    return base_class::RegisterCallback("element-removed", G_CALLBACK(cb), user_data);
  }

  gboolean RegisterDeepElementAdded(element_deep_added_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT {
    return base_class::RegisterCallback("deep-element-added", G_CALLBACK(cb), user_data);
  }

  gboolean RegisterDeepElementRemoved(element_deep_removed_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT {
    return base_class::RegisterCallback("deep-element-removed", G_CALLBACK(cb), user_data);
  }
};

// decoders elements
class ElementDecodebin : public ElementBinEx<ELEMENT_DECODEBIN> {
 public:
  typedef ElementBinEx<ELEMENT_DECODEBIN> base_class;
  using base_class::base_class;

  typedef gboolean (*autoplug_continue_callback_t)(GstElement* elem, GstPad* pad, GstCaps* caps, gpointer user_data);

  typedef GValueArray* (*autoplug_sort_callback_t)(GstElement* bin,
                                                   GstPad* pad,
                                                   GstCaps* caps,
                                                   GValueArray* factories,
                                                   gpointer user_data);
  typedef GstAutoplugSelectResult (*autoplug_select_callback_t)(GstElement* bin,
                                                                GstPad* pad,
                                                                GstCaps* caps,
                                                                GstElementFactory* factory,
                                                                gpointer user_data);

  void SetUseBuffering(bool use_buffering = false);  // Default: false

  void SetMaxSizeBuffers(guint val = 200);         // 0 - 4294967295 Default: 200
  void SetMaxSizeTime(guint val = 10485760);       // 0 - 4294967295 Default: 10485760
  void SetMaxSizeBytes(guint64 val = 1000000000);  // 0 - 18446744073709551615 Default: 1000000000

  void SetEmpty();

  gboolean RegisterAutoplugContinue(autoplug_continue_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
  gboolean RegisterAutoplugSelect(autoplug_select_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
  gboolean RegisterAutoplugSort(autoplug_sort_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
};

class ElementVaapiDecodebin : public ElementBinEx<ELEMENT_VAAPI_DECODEBIN> {
 public:
  typedef ElementBinEx<ELEMENT_VAAPI_DECODEBIN> base_class;
  using base_class::base_class;
};

class ElementVaapiPostProc : public ElementBinEx<ELEMENT_VAAPI_POST_PROC> {
 public:
  typedef ElementBinEx<ELEMENT_VAAPI_POST_PROC> base_class;
  using base_class::base_class;

  void SetDinterlaceMode(gint deinterlace_method = 0);  // Default value: Auto Detection (0),
                                                        // Allowed values: Auto Detection (0),
                                                        //                 Force Deinterlace (1),
                                                        //                 Never Deinterlace (2)

  void SetFormat(gint format = 1);                // Default value: Encoded (1),
  void SetWidth(guint width = 0);                 // Range: 0 - 2147483647 Default: 0
  void SetHeight(guint height = 0);               // Range: 0 - 2147483647 Default: 0
  void SetForceAspectRatio(bool ratio = true);    // Default: true
  void SetScaleMethod(guint met = 0);             // Range: 1 - 2 Default: 0
  void SetDeinerlaceMethod(guint met = 1);        // Range: 1 - 4 Default: 1, "bob"
  void SetDenoise(gfloat denoise = 0);            // Range: [0,1] Default: 0"
  void SetSkinToneEnhancement(bool val = false);  // Default: false
};

class ElementMFXVpp : public ElementEx<ELEMENT_MFX_VPP> {
 public:
  typedef ElementEx<ELEMENT_MFX_VPP> base_class;
  using base_class::base_class;
  void SetForceAspectRatio(bool ratio = true);  // Default: true
  void SetWidth(gint width = 0);                // Range: 0 - 2147483647 Default: 0
  void SetHeight(gint height = 0);              // Range: 0 - 2147483647 Default: 0
  void SetFrameRate(int framerate = 0);         // Range: 0 - 2147483647 Default: 0
  void SetDinterlaceMode(int mode);
};

class ElementMFXH264Decode : public ElementEx<ELEMENT_MFX_H264_DEC> {
 public:
  typedef ElementEx<ELEMENT_MFX_H264_DEC> base_class;
  using base_class::base_class;
  void SetLiveMode(bool mode = false);  // Default: false
};

class ElementAvdecH264 : public ElementEx<ELEMENT_AVDEC_H264> {
 public:
  typedef ElementEx<ELEMENT_AVDEC_H264> base_class;
  using base_class::base_class;

  void SetMaxThreads(gint threads = 0);  // Default value: 0 (auto), Allowed
                                         // values: [0, 2147483647]

  void SetSkipFrame(gint skip_frame = 0);  // Default vaule:  Skip Nothing (0)
                                           // Allowed values: Skip Nothing (0),
                                           //                 Skip B-Frames (1),
                                           //                 Skip IDCT-Dequantization
                                           //                 (2) Skip everything (5)

  void SetOutputCorrupt(gboolean output_corrupt = true);  // Default value: true
};

class ElementAvdecAc3 : public ElementEx<ELEMENT_AVDEC_AC3> {
 public:
  typedef ElementEx<ELEMENT_AVDEC_AC3> base_class;
  using base_class::base_class;
};

class ElementAvdecAc3Fixed : public ElementEx<ELEMENT_AVDEC_AC3_FIXED> {
 public:
  typedef ElementEx<ELEMENT_AVDEC_AC3_FIXED> base_class;
  using base_class::base_class;
};

// demuxer elements
class ElementHlsDemux : public ElementBinEx<ELEMENT_HLS_DEMUX> {
 public:
  typedef ElementBinEx<ELEMENT_HLS_DEMUX> base_class;
  using base_class::base_class;
};

class ElementTsDemux : public ElementBinEx<ELEMENT_TS_DEMUX> {
 public:
  typedef ElementBinEx<ELEMENT_TS_DEMUX> base_class;
  using base_class::base_class;

  void SetParsePrivateSections(gboolean parse_private_sections = true);  // Default value: true
  void SetProgramNumber(gint number);                                    // -1 - 2147483647 Default: -1
};

// common elements
class ElementQueue : public ElementEx<ELEMENT_QUEUE> {
 public:
  typedef ElementEx<ELEMENT_QUEUE> base_class;
  using base_class::base_class;

  void SetMaxSizeBuffers(guint val = 200);         // 0 - 4294967295 Default: 200
  void SetMaxSizeTime(guint val = 10485760);       // 0 - 4294967295 Default: 10485760
  void SetMaxSizeBytes(guint64 val = 1000000000);  // 0 - 18446744073709551615 Default: 1000000000

  void SetEmpty();
};

class ElementQueue2 : public ElementEx<ELEMENT_QUEUE2> {
 public:
  typedef ElementEx<ELEMENT_QUEUE2> base_class;
  using base_class::base_class;

  void SetMaxSizeBuffers(guint val = 100);           // 0 - 4294967295 Default: 100
  void SetMaxSizeTime(guint64 val = 2000000000);     // 0 - 18446744073709551615 Default: 2000000000
  void SetMaxSizeBytes(guint val = 2097152);         // 0 - 4294967295 Default: 2097152
  void SetUseBuffering(bool use_buffering = false);  // true - false: false
};

class ElementTee : public ElementEx<ELEMENT_TEE> {
 public:
  typedef ElementEx<ELEMENT_TEE> base_class;
  using base_class::base_class;
};

class ElementCapsFilter : public ElementEx<ELEMENT_CAPS_FILTER> {
 public:
  typedef ElementEx<ELEMENT_CAPS_FILTER> base_class;
  using base_class::base_class;

  void SetCaps(GstCaps* caps);
};

template <typename T>
T* make_element(const std::string& name) {
  T* element = new T(name);
  return element;
}

}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
