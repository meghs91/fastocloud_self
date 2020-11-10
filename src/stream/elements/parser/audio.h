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

class ElementAACParse : public ElementBaseParse<ELEMENT_AAC_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_AAC_PARSE> base_class;
  using base_class::base_class;
};

class ElementAC3Parse : public ElementBaseParse<ELEMENT_AC3_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_AC3_PARSE> base_class;
  using base_class::base_class;
};

class ElementMPEGAudioParse : public ElementBaseParse<ELEMENT_MPEG_AUDIO_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_MPEG_AUDIO_PARSE> base_class;
  using base_class::base_class;
};

class ElementRawAudioParse : public ElementBaseParse<ELEMENT_RAW_AUDIO_PARSE> {
 public:
  typedef ElementBaseParse<ELEMENT_RAW_AUDIO_PARSE> base_class;
  using base_class::base_class;
};

ElementAACParse* make_aac_parser(element_id_t parser_id);
ElementAC3Parse* make_ac3_parser(element_id_t parser_id);
ElementMPEGAudioParse* make_mpeg_parser(element_id_t parser_id);
ElementRawAudioParse* make_raw_parser(element_id_t parser_id);

Element* make_audio_parser(const std::string& parser, const std::string& name);

}  // namespace parser
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
