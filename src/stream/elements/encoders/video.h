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

#include <string>

#include <common/sprintf.h>

#include "stream/gst_types.h"
#include "stream/ilinker.h"  // for ILinker (ptr only), elements_line_t

#include "base/types.h"
#include "stream/stypes.h"

#include "stream/elements/element.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace encoders {

class ElementMsdkH264Enc : public ElementEx<ELEMENT_MSDK_H264_ENC> {
 public:
  typedef ElementEx<ELEMENT_MSDK_H264_ENC> base_class;
  using base_class::base_class;
};

class ElementNvH264Enc : public ElementEx<ELEMENT_NV_H264_ENC> {
 public:
  typedef ElementEx<ELEMENT_NV_H264_ENC> base_class;
  using base_class::base_class;
};

class ElementNvH265Enc : public ElementEx<ELEMENT_NV_H265_ENC> {
 public:
  typedef ElementEx<ELEMENT_NV_H265_ENC> base_class;
  using base_class::base_class;
};

class ElementX264Enc : public ElementEx<ELEMENT_X264_ENC> {
 public:
  typedef ElementEx<ELEMENT_X264_ENC> base_class;
  using base_class::base_class;
};

class ElementX265Enc : public ElementEx<ELEMENT_X265_ENC> {
 public:
  typedef ElementEx<ELEMENT_X265_ENC> base_class;
  using base_class::base_class;
};

class ElementMPEG2Enc : public ElementEx<ELEMENT_MPEG2_ENC> {
 public:
  typedef ElementEx<ELEMENT_MPEG2_ENC> base_class;
  using base_class::base_class;
};

class ElementEAVCEnc : public ElementEx<ELEMENT_EAVC_ENC> {
 public:
  typedef ElementEx<ELEMENT_EAVC_ENC> base_class;
  using base_class::base_class;
};

class ElementOpenH264Enc : public ElementEx<ELEMENT_OPEN_H264_ENC> {
 public:
  typedef ElementEx<ELEMENT_OPEN_H264_ENC> base_class;
  using base_class::base_class;
};

class ElementVAAPIH264Enc : public ElementEx<ELEMENT_VAAPI_H264_ENC> {
 public:
  typedef ElementEx<ELEMENT_VAAPI_H264_ENC> base_class;
  using base_class::base_class;
};

class ElementVAAPIMpeg2Enc : public ElementEx<ELEMENT_VAAPI_MPEG2_ENC> {
 public:
  typedef ElementEx<ELEMENT_VAAPI_MPEG2_ENC> base_class;
  using base_class::base_class;
};

class ElementMFXH264Enc : public ElementEx<ELEMENT_MFX_H264_ENC> {
 public:
  typedef ElementEx<ELEMENT_MFX_H264_ENC> base_class;
  using base_class::base_class;
  void SetIDRInterval(guint idr = 0);  // Range: 0 - 2147483647 Default: 0
};

Element* build_video_scale(int width, int height, ILinker* linker, Element* link_to, element_id_t video_scale_id);
Element* build_video_framerate(int framerate, ILinker* linker, Element* link_to, element_id_t video_framerate_id);

template <typename T>
T* make_video_encoder(element_id_t encoder_id) {
  return make_element<T>(common::MemSPrintf(VIDEO_CODEC_NAME_1U, encoder_id));
}

ElementX264Enc* make_h264_encoder(element_id_t encoder_id);
ElementX265Enc* make_h265_encoder(element_id_t encoder_id);
ElementMPEG2Enc* make_mpeg2_encoder(element_id_t encoder_id);
ElementEAVCEnc* make_eavc_encoder(element_id_t encoder_id);
ElementOpenH264Enc* make_openh264_encoder(element_id_t encoder_id);
ElementVAAPIH264Enc* make_vaapi_h264_encoder(element_id_t encoder_id);
ElementVAAPIMpeg2Enc* make_vaapi_mpeg2_encoder(element_id_t encoder_id);
ElementMFXH264Enc* make_mfx_h264_encoder(element_id_t encoder_id);
ElementNvH264Enc* make_nv_h264_encoder(element_id_t encoder_id);
ElementNvH265Enc* make_nv_h265_encoder(element_id_t encoder_id);
ElementMsdkH264Enc* make_msdk_h264_encoder(element_id_t encoder_id);

Element* make_video_encoder(const std::string& codec, const std::string& name);

elements_line_t build_video_convert(deinterlace_t deinterlace, ILinker* linker, element_id_t video_convert_id);

elements_line_t build_video_encoder(const std::string& codec,
                                    bit_rate_t video_bitrate,
                                    const video_encoders_args_t& video_args,
                                    const video_encoders_str_args_t& video_str_args,
                                    ILinker* linker,
                                    element_id_t encoder_id);

bool IsH264Encoder(const std::string& encoder);

}  // namespace encoders
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
