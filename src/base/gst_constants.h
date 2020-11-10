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

#include <array>

#define DECODEBIN "decodebin"
#define FAKE_SINK "fakesink"
#define TEST_SINK "testsink"
#define VIDEO_TEST_SRC "videotestsrc"
#define AUDIO_TEST_SRC "audiotestsrc"
#define DISPLAY_SRC "ximagesrc"
#define VIDEO_SCREEN_SINK "autovideosink"
#define AUDIO_SCREEN_SINK "autoaudiosink"
#define QUEUE "queue"
#define QUEUE2 "queue2"
#define H264_PARSE "h264parse"
#define H265_PARSE "h265parse"
#define MPEG_VIDEO_PARSE "mpegvideoparse"
#define AAC_PARSE "aacparse"
#define AC3_PARSE "ac3parse"
#define MPEG_AUDIO_PARSE "mpegaudioparse"
#define RAW_AUDIO_PARSE "rawaudioparse"
#define TEE "tee"
#define MP4_MUX "mp4mux"
#define QT_MUX "qtmux"
#define FLV_MUX "flvmux"
#define MPEGTS_MUX "mpegtsmux"
#define FILE_SINK "filesink"
#define RTP_MUX "rtpmux"
#define RTP_MPEG2_PAY "rtpmp2tpay"
#define RTP_H264_PAY "rtph264pay"
#define RTP_H265_PAY "rtph265pay"
#define RTP_AAC_PAY "rtpmp4apay"
#define RTP_AC3_PAY "rtpac3pay"
#define RTP_MPEG2_DEPAY "rtpmp2tdepay"
#define RTP_H264_DEPAY "rtph264depay"
#define RTP_H265_DEPAY "rtph265depay"
#define RTP_AAC_DEPAY "rtpmp4adepay"
#define RTP_AC3_DEPAY "rtpac3depay"
#define V4L2_SRC "v4l2src"
#define SPLIT_MUX_SINK "splitmuxsink"
#define ALSA_SRC "alsasrc"
#define MULTIFILE_SRC "multifilesrc"
#define APP_SRC "appsrc"
#define FILE_SRC "filesrc"
#define IMAGE_FREEZE "imagefreeze"
#define CAPS_FILTER "capsfilter"
#define AUDIO_CONVERT "audioconvert"
#define RG_VOLUME "rgvolume"
#define VOLUME "volume"
#define FAAC "faac"
#define VOAAC_ENC "voaacenc"
#define AUDIO_RESAMPLE "audioresample"
#define LAME_MP3_ENC "lamemp3enc"
#define VIDEO_CONVERT "videoconvert"
#define AV_DEINTERLACE "avdeinterlace"
#define DEINTERLACE "deinterlace"
#define ASPECT_RATIO "aspectratiocrop"
#define UDP_SINK "udpsink"
#define TCP_SERVER_SINK "tcpserversink"
#define RTMP_SINK "rtmpsink"
#define HLS_SINK "hlssink"
#define SOUP_HTTP_SRC "souphttpsrc"
#define DVB_SRC "dvbsrc"
#define VIDEO_SCALE "videoscale"
#define VIDEO_RATE "videorate"
#define MULTIFILE_SINK "multifilesink"
#define KVS_SINK "kvssink"

#define NV_H264_ENC "nvh264enc"
#define NV_H264_ENC_PARAM(x) NV_H264_ENC "." x
#define NV_H264_ENC_PRESET NV_H264_ENC_PARAM("preset")

#define NV_H265_ENC "nvh265enc"
#define NV_H265_ENC_PARAM(x) NV_H265_ENC "." x
#define NV_H265_ENC_PRESET NV_H265_ENC_PARAM("preset")

#define MSDK_H264_ENC "msdkh264enc"

#define X264_ENC "x264enc"
#define X264_ENC_PARAM(x) X264_ENC "." x
#define X264_ENC_SPEED_PRESET X264_ENC_PARAM("speed-preset")
#define X264_ENC_THREADS X264_ENC_PARAM("threads")
#define X264_ENC_TUNE X264_ENC_PARAM("tune")
#define X264_ENC_KEY_INT_MAX X264_ENC_PARAM("key-int-max")
#define X264_ENC_RC_LOOKAHED X264_ENC_PARAM("rc-lookahead")
#define X264_ENC_QP_MAX X264_ENC_PARAM("qp-max")
#define X264_ENC_PASS X264_ENC_PARAM("pass")
#define X264_ENC_ME X264_ENC_PARAM("me")
#define X264_ENC_PROFILE X264_ENC_PARAM("profile")
#define X264_ENC_STREAM_FORMAT X264_ENC_PARAM("stream-format")
#define X264_ENC_OPTION_STRING X264_ENC_PARAM("option-string")
#define X264_ENC_INTERLACED X264_ENC_PARAM("interlaced")
#define X264_ENC_DCT8X8 X264_ENC_PARAM("dct8x8")
#define X264_ENC_B_ADAPT X264_ENC_PARAM("b-adapt")
#define X264_ENC_BYTE_STREAM X264_ENC_PARAM("byte-stream")
#define X264_ENC_CABAC X264_ENC_PARAM("cabac")
#define X264_ENC_SLICED_THREADS X264_ENC_PARAM("sliced-threads")
#define X264_ENC_VBV_BUF_CAPACITY X264_ENC_PARAM("vbv-buf-capacity")
#define X264_ENC_QUANTIZER X264_ENC_PARAM("quantizer")

#define X265_ENC "x265enc"
#define X265_ENC_PARAM(x) X265_ENC "." x
#define MPEG2_ENC "mpeg2enc"
#define MPEG2_ENC_PARAM(x) MPEG2_ENC "." x

#define EAVC_ENC "eavcenc"
#define EAVC_ENC_PARAM(x) EAVC_ENC "." x
#define EAVC_ENC_PRESET EAVC_ENC_PARAM("preset")
#define EAVC_ENC_PROFILE EAVC_ENC_PARAM("profile")
#define EAVC_ENC_PERFORMANCE EAVC_ENC_PARAM("performance")
#define EAVC_ENC_BITRATE_MODE EAVC_ENC_PARAM("bitrate-mode")
#define EAVC_ENC_BITRATE_PASS EAVC_ENC_PARAM("bitrate-pass")
#define EAVC_ENC_BITRATE_MAX EAVC_ENC_PARAM("bitrate-max")
#define EAVC_ENC_VBV_SIZE EAVC_ENC_PARAM("vbv-size")
#define EAVC_ENC_PICTURE_MODE EAVC_ENC_PARAM("picture-mode")
#define EAVC_ENC_ENTROPY_MODE EAVC_ENC_PARAM("entropy-mode")
#define EAVC_ENC_GOP_MAX_BCOUNT EAVC_ENC_PARAM("gop-max-bcount")
#define EAVC_ENC_GOP_MAX_LENGTH EAVC_ENC_PARAM("gop-max-length")
#define EAVC_ENC_GOP_MIN_LENGTH EAVC_ENC_PARAM("gop-min-length")
#define EAVC_ENC_LEVEL EAVC_ENC_PARAM("level")
#define EAVC_ENC_DEBLOCK_MODE EAVC_ENC_PARAM("deblock-mode")
#define EAVC_ENC_DEBLOCK_ALPHA EAVC_ENC_PARAM("deblock-alpha")
#define EAVC_ENC_DEBLOCK_BETA EAVC_ENC_PARAM("deblock-beta")
#define EAVC_ENC_INITIAL_DELAY EAVC_ENC_PARAM("initial-param")
#define EAVC_ENC_FIELD_ORDER EAVC_ENC_PARAM("field-order")
#define EAVC_ENC_GOP_ADAPTIVE EAVC_ENC_PARAM("gop-adaptive")

#define OPEN_H264_ENC "openh264enc"
#define OPEN_H264_ENC_PARAM(x) OPEN_H264_ENC "." x
#define OPEN_H264_ENC_MUTLITHREAD OPEN_H264_ENC_PARAM("multi-thread")
#define OPEN_H264_ENC_RATE_CONTROL OPEN_H264_ENC_PARAM("rate-control")
#define OPEN_H264_ENC_GOP_SIZE OPEN_H264_ENC_PARAM("gop-size")
#define OPEN_H264_ENC_COMPLEXITY OPEN_H264_ENC_PARAM("complexity")

#define UDP_SRC "udpsrc"
#define RTMP_SRC "rtmpsrc"
#define RTSP_SRC "rtspsrc"
#define TCP_SERVER_SRC "tcpserversrc"

#define VAAPI_H264_ENC "vaapih264enc"
#define VAAPI_H264_ENC_PARAM(x) VAAPI_H264_ENC "." x
#define VAAPI_H264_ENC_KEYFRAME_PERIOD VAAPI_H264_ENC_PARAM("keyframe-period")
#define VAAPI_H264_ENC_TUNE VAAPI_H264_ENC_PARAM("tune")
#define VAAPI_H264_ENC_MAX_BFRAMES VAAPI_H264_ENC_PARAM("max-bframes")
#define VAAPI_H264_ENC_NUM_SLICES VAAPI_H264_ENC_PARAM("num-slices")
#define VAAPI_H264_ENC_INIT_QP VAAPI_H264_ENC_PARAM("init-qp")
#define VAAPI_H264_ENC_MIN_QP VAAPI_H264_ENC_PARAM("min-qp")
#define VAAPI_H264_ENC_RATE_CONTROL VAAPI_H264_ENC_PARAM("rate-control")
#define VAAPI_H264_ENC_CABAC VAAPI_H264_ENC_PARAM("cabac")
#define VAAPI_H264_ENC_DCT8X8 VAAPI_H264_ENC_PARAM("dct8x8")
#define VAAPI_H264_ENC_B_ADAPT VAAPI_H264_ENC_PARAM("b-adapt")
#define VAAPI_H264_ENC_CPB_LENGTH VAAPI_H264_ENC_PARAM("cpb-length")

#define VAAPI_MPEG2_ENC "vaapimpeg2enc"
#define VAAPI_DECODEBIN "vaapidecodebin"
#define VAAPI_POST_PROC "vaapipostproc"

#define GDK_PIXBUF_OVERLAY "gdkpixbufoverlay"
#define RSVG_OVERLAY "rsvgoverlay"
#define VIDEO_BOX "videobox"
#define VIDEO_MIXER "videomixer"
#define AUDIO_MIXER "audiomixer"
#define INTERLEAVE "interleave"
#define DEINTERLEAVE "deinterleave"
#define CAIRO_OVERLAY "cairooverlay"
#define TEXT_OVERLAY "textoverlay"
#define VIDEO_CROP "videocrop"
#define SPECTRUM "spectrum"
#define LEVEL "level"
#define HLS_DEMUX "hlsdemux"
#define VIDEO_DECK_SINK "decklinkvideosink"
#define AUDIO_DECK_SINK "decklinkaudiosink"
#define INTERLACE "interlace"
#define AUTO_VIDEO_CONVERT "autovideoconvert"
#define TS_PARSE "tsparse"
#define AVDEC_H264 "avdec_h264"
#define TS_DEMUX "tsdemux"
#define TS_DEMUX_PARAM(x) TS_DEMUX "." x
#define TS_DEMUX_PROGRAM_NUMBER TS_DEMUX_PARAM("program-number")

#define AVDEC_AC3 "avdec_ac3"
#define AVDEC_AC3_FIXED "avdec_ac3_fixed"
#define AVDEC_AAC "avdec_aac"
#define AVDEC_AAC_FIXED "avdec_aac_fixed"
#define SOUP_HTTP_CLIENT_SINK "souphttpclientsink"

#define MFX_H264_ENC "mfxh264enc"
#define MFX_H264_ENC_PARAM(x) MFX_H264_ENC "." x
#define MFX_H264_ENC_PRESET MFX_H264_ENC_PARAM("preset")
#define MFX_H264_GOP_SIZE MFX_H264_ENC_PARAM("gop-size")
#define MFX_VPP "mfxvpp"
#define MFX_H264_DEC "mfxh264dec"

#define SRT_SRC "srtsrc"
#define SRT_SINK "srtsink"

// deep learning
#define TINY_YOLOV2 "tinyyolov2"
#define TINY_YOLOV3 "tinyyolov3"
#define DETECTION_OVERLAY "detectionoverlay"

#define SUPPORTED_VIDEO_PARSERS_COUNT 3
#define SUPPORTED_AUDIO_PARSERS_COUNT 4

extern const std::array<const char*, SUPPORTED_VIDEO_PARSERS_COUNT> kSupportedVideoParsers;
extern const std::array<const char*, SUPPORTED_AUDIO_PARSERS_COUNT> kSupportedAudioParsers;

#define SUPPORTED_VIDEO_ENCODERS_COUNT 10
#define SUPPORTED_AUDIO_ENCODERS_COUNT 3

extern const std::array<const char*, SUPPORTED_VIDEO_ENCODERS_COUNT> kSupportedVideoEncoders;
extern const std::array<const char*, SUPPORTED_AUDIO_ENCODERS_COUNT> kSupportedAudioEncoders;
