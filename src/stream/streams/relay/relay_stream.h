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

#include "stream/streams/src_decodebin_stream.h"

#include "stream/streams/configs/relay_config.h"

namespace fastocloud {
namespace stream {
namespace streams {

class RelayStream : public SrcDecodeBinStream {
 public:
  RelayStream(const RelayConfig* config, IStreamClient* client, StreamStruct* stats);

  const char* ClassName() const override;

 protected:
  IBaseBuilder* CreateBuilder() override;

  gboolean HandleDecodeBinAutoplugger(GstElement* elem, GstPad* pad, GstCaps* caps) override;
  void HandleDecodeBinPadAdded(GstElement* src, GstPad* new_pad) override;

  GValueArray* HandleAutoplugSort(GstElement* bin, GstPad* pad, GstCaps* caps, GValueArray* factories) override;
  GstAutoplugSelectResult HandleAutoplugSelect(GstElement* bin,
                                               GstPad* pad,
                                               GstCaps* caps,
                                               GstElementFactory* factory) override;

  void HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) override;
  void HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) override;
};

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
