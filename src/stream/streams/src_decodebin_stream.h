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

#include "stream/ibase_stream.h"

#include "stream/elements/element.h"

namespace fastocloud {
namespace stream {
namespace streams {

namespace builders {
class SrcDecodeStreamBuilder;
}

class SrcDecodeBinStream : public IBaseStream {
  friend class builders::SrcDecodeStreamBuilder;

 public:
  SrcDecodeBinStream(const Config* config, IStreamClient* client, StreamStruct* stats);

  const char* ClassName() const override;

 protected:
  void OnInpudSrcPadCreated(pad::Pad* src_pad, element_id_t id, const common::uri::GURL& url) override;
  void OnOutputSinkPadCreated(pad::Pad* sink_pad,
                              element_id_t id,
                              const common::uri::GURL& url,
                              bool need_push) override;
  virtual void OnDecodebinCreated(elements::ElementDecodebin* decodebin);

  IBaseBuilder* CreateBuilder() override = 0;

  void PreLoop() override;
  void PostLoop(ExitStatus status) override;

  virtual void ConnectDecodebinSignals(elements::ElementDecodebin* decodebin);

  virtual gboolean HandleDecodeBinAutoplugger(GstElement* elem, GstPad* pad, GstCaps* caps) = 0;
  virtual void HandleDecodeBinPadAdded(GstElement* src, GstPad* new_pad) = 0;
  virtual GValueArray* HandleAutoplugSort(GstElement* bin, GstPad* pad, GstCaps* caps, GValueArray* factories) = 0;
  virtual GstAutoplugSelectResult HandleAutoplugSelect(GstElement* bin,
                                                       GstPad* pad,
                                                       GstCaps* caps,
                                                       GstElementFactory* factory) = 0;

  virtual void HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) = 0;
  virtual void HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) = 0;

 private:
  static void decodebin_pad_added_callback(GstElement* src, GstPad* new_pad, gpointer user_data);
  static gboolean decodebin_autoplugger_callback(GstElement* elem, GstPad* pad, GstCaps* caps, gpointer user_data);

  static GstAutoplugSelectResult decodebin_autoplug_select_callback(GstElement* bin,
                                                                    GstPad* pad,
                                                                    GstCaps* caps,
                                                                    GstElementFactory* factory,
                                                                    gpointer user_data);

  static GValueArray* decodebin_autoplug_sort_callback(GstElement* bin,
                                                       GstPad* pad,
                                                       GstCaps* caps,
                                                       GValueArray* factories,
                                                       gpointer user_data);

  static void decodebin_element_added_callback(GstBin* bin, GstElement* element, gpointer user_data);
  static void decodebin_element_removed_callback(GstBin* bin, GstElement* element, gpointer user_data);
};

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
