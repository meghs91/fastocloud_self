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

#include "stream/streams/builders/gst_base_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

GstBaseBuilder::GstBaseBuilder(const Config* api, IBaseBuilderObserver* observer) : IBaseBuilder(api, observer) {}

bool GstBaseBuilder::InitPipeline() {
  Connector conn = BuildInput();
  conn = BuildUdbConnections(conn);
  conn = BuildPostProc(conn);
  conn = BuildConverter(conn);
  BuildOutput(conn);
  return true;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
