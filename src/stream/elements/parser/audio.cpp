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

#include "stream/elements/parser/audio.h"

#include <string>

#include <common/sprintf.h>

namespace fastocloud {
namespace stream {
namespace elements {
namespace parser {

namespace {
template <typename T>
T* make_audio_parser(element_id_t parser_id) {
  return make_element<T>(common::MemSPrintf(AUDIO_PARSER_NAME_1U, parser_id));
}
}  // namespace

ElementAACParse* make_aac_parser(element_id_t parser_id) {
  return make_audio_parser<ElementAACParse>(parser_id);
}

ElementMPEGAudioParse* make_mpeg_parser(element_id_t parser_id) {
  return make_audio_parser<ElementMPEGAudioParse>(parser_id);
}

ElementAC3Parse* make_ac3_parser(element_id_t parser_id) {
  return make_audio_parser<ElementAC3Parse>(parser_id);
}

ElementRawAudioParse* make_raw_parser(element_id_t parser_id) {
  return make_audio_parser<ElementRawAudioParse>(parser_id);
}

Element* make_audio_parser(const std::string& parser, const std::string& name) {
  if (parser == ElementAACParse::GetPluginName()) {
    return new ElementAACParse(name);
  } else if (parser == ElementAC3Parse::GetPluginName()) {
    return new ElementAC3Parse(name);
  } else if (parser == ElementMPEGAudioParse::GetPluginName()) {
    return new ElementMPEGAudioParse(name);
  } else if (parser == ElementRawAudioParse::GetPluginName()) {
    return new ElementRawAudioParse(name);
  }

  NOTREACHED() << "Please register new audio parser type: " << parser;
  return nullptr;
}

}  // namespace parser
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
