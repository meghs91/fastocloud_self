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
#include <vector>

namespace fastocloud {
namespace stream {

namespace elements {
class Element;
}

typedef std::vector<elements::Element*> elements_line_t;

struct Connector {
  elements::Element* video;
  elements::Element* audio;
  elements::Element* subtitle;
};

enum EncoderType { GPU_MFX, GPU_VAAPI, GPU_NVIDIA, CPU };

bool GetEncoderType(const std::string& encoder, EncoderType* enc);

}  // namespace stream
}  // namespace fastocloud
