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

#include "stream/elements/sources/multifilesrc.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementMultiFileSrc::SetIndex(int index) {
  SetProperty("index", index);
}

void ElementMultiFileSrc::SetLoop(bool loop) {
  SetProperty("loop", loop);
}

ElementMultiFileSrc* make_multifile_src(const MultiFileSrcInfo& info, element_id_t input_id) {
  ElementMultiFileSrc* multifile_src = make_sources<ElementMultiFileSrc>(input_id);
  multifile_src->SetLocation(info.location);
  multifile_src->SetIndex(info.index);
  multifile_src->SetLoop(info.loop);
  return multifile_src;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
