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

#include "stream/ibase_builder.h"

#include "stream/gst_types.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

// input => prepostproc (udb) => | postproc (raw image) | => convert => output

class GstBaseBuilder : public IBaseBuilder {
 public:
  GstBaseBuilder(const Config* api, IBaseBuilderObserver* observer);

  virtual Connector BuildInput() = 0;

  virtual Connector BuildUdbConnections(Connector conn) = 0;
  virtual Connector BuildPostProc(Connector conn) = 0;
  virtual Connector BuildConverter(Connector conn) = 0;
  virtual Connector BuildOutput(Connector conn) = 0;

 protected:
  bool InitPipeline() override final;
};

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
