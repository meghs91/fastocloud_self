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

#include "stream/probes.h"

#include "stream/ibase_stream.h"

namespace fastocloud {
namespace stream {

Consistency::Consistency()
    : segment(FALSE),
      eos(TRUE),
      expect_flush(0),
      flushing(FALSE),
      saw_stream_start(FALSE),
      saw_serialized_event(FALSE) {}

Probe::Probe(element_id_t id, const common::uri::GURL& url, IBaseStream* stream)
    : stream_(stream), id_(id), id_buffer_(0), pad_(nullptr), consistency_(), url_(url) {
  CHECK(stream);
}

Probe::~Probe() {
  Clear();
}

const common::uri::GURL& Probe::GetUrl() const {
  return url_;
}

void Probe::Clear() {
  if (!pad_) {
    return;
  }

  gst_pad_remove_probe(pad_, id_buffer_);
}

void Probe::ClearInner() {
  pad_ = nullptr;
  id_buffer_ = 0;
}

element_id_t Probe::GetID() const {
  return id_;
}

GstPad* Probe::GetPad() const {
  return pad_;
}

Consistency Probe::GetConsistency() const {
  return consistency_;
}

void Probe::destroy_callback_probe(gpointer user_data) {
  Probe* probe = reinterpret_cast<Probe*>(user_data);
  probe->ClearInner();
}

InputProbe::InputProbe(element_id_t id, const common::uri::GURL& url, IBaseStream* stream)
    : base_class(id, url, stream) {}

GstPadProbeReturn InputProbe::source_callback_probe_buffer(GstPad* pad, GstPadProbeInfo* info, gpointer user_data) {
  InputProbe* probe = reinterpret_cast<InputProbe*>(user_data);
  IBaseStream* stream = probe->stream_;
  GstPadProbeInfo* checked_info = stream->CheckProbeData(probe, info);
  if (!checked_info) {
    return GST_PAD_PROBE_DROP;
  }

  void* data = GST_PAD_PROBE_INFO_DATA(checked_info);
  if (GST_IS_BUFFER(data)) {
    GstBuffer* buffer = GST_PAD_PROBE_INFO_BUFFER(checked_info);
    stream->UpdateInputProbeStats(probe, gst_buffer_get_size(buffer));
  } else if (GST_IS_EVENT(data)) {
    GstEvent* event = GST_EVENT(data);
    const gchar* event_name = GST_EVENT_TYPE_NAME(event);
    GstEventType event_type = GST_EVENT_TYPE(event);
    DEBUG_LOG() << "Source[" << probe->id_ << "] event: " << event_name;

    if (event_type == GST_EVENT_FLUSH_START) {
      /* getting two flush_start in a row seems to be okay
     fail_if (consist->flushing, "Received another FLUSH_START");
  */
      probe->consistency_.flushing = TRUE;
    } else if (event_type == GST_EVENT_FLUSH_STOP) {
      /* Receiving a flush-stop is only valid after receiving a flush-start */
      if (!probe->consistency_.flushing) {
        INFO_LOG() << "Received a FLUSH_STOP without a FLUSH_START on pad " << pad;
      }
      if (probe->consistency_.eos) {
        INFO_LOG() << "Received a FLUSH_STOP after an EOS on pad " << pad;
      }
      probe->consistency_.flushing = probe->consistency_.expect_flush = FALSE;
    } else if (event_type == GST_EVENT_STREAM_START) {
      if (probe->consistency_.saw_serialized_event && !probe->consistency_.saw_stream_start) {
        INFO_LOG() << "Got a STREAM_START event after a serialized event on pad " << pad;
      }
      probe->consistency_.saw_stream_start = TRUE;
    } else if (event_type == GST_EVENT_CAPS) {
      /* ok to have these before segment event */
      /* FIXME check order more precisely, if so spec'ed somehow ? */
      GstCaps* caps = nullptr;
      gst_event_parse_caps(event, &caps);
      if (caps) {
        GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
        if (pad_struct) {
          gchar* structure_text = gst_structure_to_string(pad_struct);
          INFO_LOG() << "Source[" << probe->id_ << "] caps are: " << structure_text;
          g_free(structure_text);
        }
      }
    } else if (event_type == GST_EVENT_SEGMENT) {
      if (probe->consistency_.expect_flush && probe->consistency_.flushing) {
        INFO_LOG() << "Received SEGMENT while in a flushing seek on pad " << pad;
      }
      const GstSegment* segment = nullptr;
      gst_event_parse_segment(event, &segment);
      probe->consistency_.segment = TRUE;
      probe->consistency_.eos = FALSE;
    } else if (event_type == GST_EVENT_EOS) {
      /* FIXME : not 100% sure about whether two eos in a row is valid */
      if (probe->consistency_.eos) {
        INFO_LOG() << "Received EOS just after another EOS on pad " << pad;
      }
      probe->consistency_.eos = TRUE;
      probe->consistency_.segment = FALSE;
    } else {
      if (GST_EVENT_IS_SERIALIZED(event) && GST_EVENT_IS_DOWNSTREAM(event)) {
        if (probe->consistency_.eos) {
          INFO_LOG() << "Event received after EOS";
        }
        if (!probe->consistency_.segment) {
          INFO_LOG() << "Event " << event_name << " received before segment on pad " << pad;
        }
      }
      /* FIXME : Figure out what to do for other events */
    }

    if (GST_EVENT_IS_SERIALIZED(event)) {
      if (!probe->consistency_.saw_stream_start && event_type != GST_EVENT_STREAM_START) {
        INFO_LOG() << "Got a serialized event (" << event_name << ") before a STREAM_START on pad" << pad;
      }
      probe->consistency_.saw_serialized_event = TRUE;
    }
    stream->HandleInputProbeEvent(probe, event);
  } else {
    GstPadProbeType pt = GST_PAD_PROBE_INFO_TYPE(checked_info);
    WARNING_LOG() << "Unknow probe type: " << pt;
  }

  return GST_PAD_PROBE_OK;
}

void InputProbe::Link(GstPad* pad) {
  if (!pad) {
    return;
  }

  Clear();

  GstPadDirection dir = gst_pad_get_direction(pad);
  gulong id_probe = 0;
  if (dir == GST_PAD_SRC) {
    id_probe = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM, source_callback_probe_buffer, this,
                                 &destroy_callback_probe);
  } else {
    NOTREACHED();
  }

  if (!id_probe) {
    CRITICAL_LOG() << "Cannot add output prode";
    return;
  }

  pad_ = pad;
  id_buffer_ = id_probe;
  DEBUG_LOG() << "Input probe added id: " << id_probe;
}

OutputProbe::OutputProbe(element_id_t id, const common::uri::GURL& url, bool need_push, IBaseStream* stream)
    : base_class(id, url, stream), need_push_(need_push) {}

GstPadProbeReturn OutputProbe::sink_callback_probe_buffer(GstPad* pad, GstPadProbeInfo* info, gpointer user_data) {
  OutputProbe* probe = reinterpret_cast<OutputProbe*>(user_data);
  IBaseStream* stream = probe->stream_;
  GstPadProbeInfo* checked_info = stream->CheckProbeDataOutput(probe, info);
  if (!checked_info) {
    return GST_PAD_PROBE_DROP;
  }

  void* data = GST_PAD_PROBE_INFO_DATA(checked_info);
  if (GST_IS_BUFFER(data)) {
    GstBuffer* buffer = GST_PAD_PROBE_INFO_BUFFER(checked_info);
    stream->UpdateOutputProbeStats(probe, gst_buffer_get_size(buffer));
  } else if (GST_IS_BUFFER_LIST(data)) {
    GstBufferList* buffer_list = GST_PAD_PROBE_INFO_BUFFER_LIST(checked_info);
    guint len = gst_buffer_list_length(buffer_list);
    for (guint i = 0; i < len; ++i) {
      GstBuffer* buffer = gst_buffer_list_get(buffer_list, i);
      stream->UpdateOutputProbeStats(probe, gst_buffer_get_size(buffer));
    }
  } else if (GST_IS_EVENT(data)) {
    GstEvent* event = GST_EVENT(data);
    const gchar* event_name = GST_EVENT_TYPE_NAME(event);
    GstEventType event_type = GST_EVENT_TYPE(event);

    DEBUG_LOG() << "Sink[" << probe->id_ << "] event: " << event_name;
    if (event_type == GST_EVENT_SEEK) {
      GstSeekFlags flags;
      gst_event_parse_seek(event, nullptr, nullptr, &flags, nullptr, nullptr, nullptr, nullptr);
      probe->consistency_.expect_flush = ((flags & GST_SEEK_FLAG_FLUSH) == GST_SEEK_FLAG_FLUSH);
    } else if (event_type == GST_EVENT_CAPS) {
      /* ok to have these before segment event */
      /* FIXME check order more precisely, if so spec'ed somehow ? */
      GstCaps* caps = nullptr;
      gst_event_parse_caps(event, &caps);
      if (caps) {
        GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
        if (pad_struct) {
          gchar* structure_text = gst_structure_to_string(pad_struct);
          INFO_LOG() << "Sink[" << probe->id_ << "] caps are: " << structure_text;
          g_free(structure_text);
        }
      }
    } else if (event_type == GST_EVENT_SEGMENT) {
      if (probe->consistency_.expect_flush && probe->consistency_.flushing) {
        INFO_LOG() << "Received SEGMENT while in a flushing seek on pad " << pad;
      }
      probe->consistency_.segment = TRUE;
      probe->consistency_.eos = FALSE;
    } else if (event_type == GST_EVENT_EOS) {
      /* FIXME : not 100% sure about whether two eos in a row is valid */
      if (probe->consistency_.eos) {
        INFO_LOG() << "Received EOS just after another EOS on pad " << pad;
      }
      probe->consistency_.eos = TRUE;
      probe->consistency_.segment = FALSE;
    }

    stream->HandleOutputProbeEvent(probe, event);
  } else {
    GstPadProbeType pt = GST_PAD_PROBE_INFO_TYPE(checked_info);
    WARNING_LOG() << "Unknow probe type: " << pt;
  }

  return GST_PAD_PROBE_OK;
}

bool OutputProbe::GetNeedPush() const {
  return need_push_;
}

void OutputProbe::Link(GstPad* pad) {
  if (!pad) {
    return;
  }

  Clear();

  GstPadDirection dir = gst_pad_get_direction(pad);
  gulong id_probe = 0;
  if (dir == GST_PAD_SINK) {
    id_probe = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM, sink_callback_probe_buffer, this,
                                 &destroy_callback_probe);
  } else {
    NOTREACHED();
  }

  if (!id_probe) {
    CRITICAL_LOG() << "Cannot add output prode";
    return;
  }

  pad_ = pad;
  id_buffer_ = id_probe;
  DEBUG_LOG() << "Output probe added id: " << id_probe;
}

}  // namespace stream
}  // namespace fastocloud
