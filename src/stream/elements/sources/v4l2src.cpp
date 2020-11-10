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

#include "stream/elements/sources/v4l2src.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementV4L2Src::SetDevice(const std::string& device) {
  SetProperty("device", device);
}

ElementV4L2Src* make_v4l2_src(const std::string& device, element_id_t input_id) {
  ElementV4L2Src* v4l2sr = make_sources<ElementV4L2Src>(input_id);
  v4l2sr->SetDevice(device);
  return v4l2sr;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
