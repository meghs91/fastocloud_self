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

#include "stream/elements/sources/srtsrc.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementSRTSrc::SetUri(const std::string& uri) {
  SetProperty("uri", uri);
}

ElementSRTSrc* make_srt_src(const std::string& uri, element_id_t input_id) {
  ElementSRTSrc* src = make_sources<ElementSRTSrc>(input_id);
  src->SetUri(uri);
  return src;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
