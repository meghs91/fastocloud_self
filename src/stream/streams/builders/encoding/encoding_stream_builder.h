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

#include "stream/streams/builders/src_decodebin_stream_builder.h"

#include "stream/streams/configs/encode_config.h"

namespace fastocloud {
namespace stream {
namespace elements {
class ElementQueue;
#if defined(MACHINE_LEARNING)
namespace machine_learning {
class ElementVideoMLFilter;
}
#endif
}  // namespace elements

namespace streams {
namespace builders {

class EncodingStreamBuilder : public SrcDecodeStreamBuilder {
 public:
  EncodingStreamBuilder(const EncodeConfig* api, SrcDecodeBinStream* observer);
  Connector BuildPostProc(Connector conn) override;
  Connector BuildConverter(Connector conn) override;

  SupportedVideoCodec GetVideoCodecType() const override;
  SupportedAudioCodec GetAudioCodecType() const override;

 protected:
  virtual elements_line_t BuildVideoPostProc(element_id_t video_id);
  virtual elements_line_t BuildAudioPostProc(element_id_t audio_id);

  virtual elements_line_t BuildVideoConverter(element_id_t video_id);
  virtual elements_line_t BuildAudioConverter(element_id_t audio_id);

#if defined(MACHINE_LEARNING)
  void HandleMLElementCreated(fastocloud::stream::elements::machine_learning::ElementVideoMLFilter* machine);
#endif
};

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
