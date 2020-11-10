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

#include "stream/streams/builders/gst_base_builder.h"

namespace fastocloud {
namespace stream {
namespace elements {
class ElementDecodebin;
}
namespace streams {
class SrcDecodeBinStream;
class AudioVideoConfig;
namespace builders {

class SrcDecodeStreamBuilder : public GstBaseBuilder {
 public:
  SrcDecodeStreamBuilder(const AudioVideoConfig* config, SrcDecodeBinStream* observer);

  Connector BuildInput() override;
  virtual elements::Element* BuildInputSrc();

  Connector BuildUdbConnections(Connector conn) override;
  virtual elements::Element* BuildVideoUdbConnection();
  virtual elements::Element* BuildAudioUdbConnection();

  Connector BuildPostProc(Connector conn) override = 0;
  Connector BuildConverter(Connector conn) override = 0;
  Connector BuildOutput(Connector conn) override;

  virtual SupportedVideoCodec GetVideoCodecType() const = 0;
  virtual SupportedAudioCodec GetAudioCodecType() const = 0;

 protected:
  void HandleDecodebinCreated(elements::ElementDecodebin* decodebin);
};

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
