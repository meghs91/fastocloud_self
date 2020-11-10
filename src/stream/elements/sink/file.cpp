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

#include "stream/elements/sink/file.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

ElementFileSink* make_file_sink(const std::string& path, element_id_t sink_id) {
  ElementFileSink* file_out = make_sink<ElementFileSink>(sink_id);
  file_out->SetLocation(path);
  return file_out;
}

void ElementFileSink::SetLocation(const std::string& location) {
  SetProperty("location", location);
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
