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

#include "stream/stypes.h"

#include "stream/elements/parser/parser.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace parser {

class ElementH264Parse : public ElementBaseParse<ELEMENT_H264_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_H264_PARSE> base_class;
  using base_class::base_class;

  void SetConfigInterval(gint interval = 0);  // Range: -1 - 3600 Default: 0
  void SetDisablePassthrough(gboolean pass);
};

class ElementH265Parse : public ElementBaseParse<ELEMENT_H265_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_H265_PARSE> base_class;
  using base_class::base_class;

  void SetConfigInterval(gint interval = 0);  // Range: -1 - 3600 Default: 0
};

class ElementMpegParse : public ElementBaseParse<ELEMENT_MPEG_VIDEO_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_MPEG_VIDEO_PARSE> base_class;
  using base_class::base_class;
};

class ElementTsParse : public ElementBaseParse<ELEMENT_TS_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_TS_PARSE> base_class;
  using base_class::base_class;
};

ElementMpegParse* make_mpeg2_parser(element_id_t parser_id);
ElementTsParse* make_ts_parser(element_id_t parser_id);
ElementH264Parse* make_h264_parser(element_id_t parser_id);
ElementH265Parse* make_h265_parser(element_id_t parser_id);

Element* make_video_parser(const std::string& parser, const std::string& name);

}  // namespace parser
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
