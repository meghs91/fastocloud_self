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

#include <common/utils.h>

#include <string>  // for string

#include <gst/gstpad.h>  // for GstPad, GstPadProbeInfo, GstPadProbeReturn

#include "stream/stypes.h"

namespace fastocloud {
namespace stream {

class IBaseStream;

struct Consistency {
  Consistency();

  gboolean segment;
  gboolean eos;
  gboolean expect_flush;
  gboolean flushing;
  gboolean saw_stream_start;
  gboolean saw_serialized_event;
};

class Probe {
 public:
  Probe(element_id_t id, const common::uri::GURL& url, IBaseStream* stream);
  virtual ~Probe();

  const common::uri::GURL& GetUrl() const;

  virtual void Link(GstPad* pad) = 0;
  element_id_t GetID() const;

  GstPad* GetPad() const;
  Consistency GetConsistency() const;

 protected:
  static void destroy_callback_probe(gpointer user_data);

  void Clear();

 private:
  void ClearInner();

 protected:
  IBaseStream* const stream_;

  const element_id_t id_;
  gulong id_buffer_;
  GstPad* pad_;
  Consistency consistency_;
  const common::uri::GURL url_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Probe);
};

class InputProbe : public Probe {
 public:
  typedef Probe base_class;
  InputProbe(element_id_t id, const common::uri::GURL& url, IBaseStream* stream);

  void Link(GstPad* pad) override;

 private:
  static GstPadProbeReturn source_callback_probe_buffer(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);
};

class OutputProbe : public Probe {
 public:
  typedef Probe base_class;
  OutputProbe(element_id_t id, const common::uri::GURL& url, bool need_push, IBaseStream* stream);

  bool GetNeedPush() const;

  void Link(GstPad* pad) override;

 private:
  static GstPadProbeReturn sink_callback_probe_buffer(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);

  const bool need_push_;
};

}  // namespace stream
}  // namespace fastocloud
