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

#include "stream/ibase_builder.h"

#include <gst/gst.h>
#include <gst/gstpipeline.h>

#include <algorithm>

#include "stream/elements/element.h"
#include "stream/elements/sink/build_output.h"
#include "stream/ibase_builder_observer.h"
#include "stream/ibase_stream.h"

#include "pad/pad.h"

namespace fastocloud {
namespace stream {

IBaseBuilder::IBaseBuilder(const Config* config, IBaseBuilderObserver* observer)
    : config_(config), observer_(observer), pipeline_(gst_pipeline_new("pipeline")), pipeline_elements_() {}

IBaseBuilder::~IBaseBuilder() {}

const Config* IBaseBuilder::GetConfig() const {
  return config_;
}

elements::Element* IBaseBuilder::GetElementByName(const std::string& name) const {
  for (elements::Element* el : pipeline_elements_) {
    if (el->GetName() == name) {
      return el;
    }
  }

  NOTREACHED() << "Not founded element name: " << name;
  return nullptr;
}

bool IBaseBuilder::ElementAdd(elements::Element* elem) {
  GstBin* pipeline = GST_BIN(pipeline_);
  bool res = gst_bin_add(pipeline, elem->GetGstElement());
  CHECK(res) << "Can't added " << elem->GetPluginName();
  pipeline_elements_.push_back(elem);
  return res;
}

bool IBaseBuilder::ElementLink(elements::Element* src, elements::Element* dest) {
  if (!src || !dest) {
    std::string src_name = src ? src->GetPluginName() : "unknown";
    std::string dest_name = dest ? dest->GetPluginName() : "unknown";
    NOTREACHED() << "src: " << src_name << ", dest: " << dest_name;
    return false;
  }

  if (!src->GetGstElement() || !dest->GetGstElement()) {
    NOTREACHED();
    return false;
  }

  bool res = gst_element_link(src->GetGstElement(), dest->GetGstElement());
  CHECK(res) << "Can't linked " << src->GetPluginName() << " to " << dest->GetPluginName();
  return res;
}

bool IBaseBuilder::ElementRemove(elements::Element* elem) {
  GstBin* pipeline = GST_BIN(pipeline_);
  bool res = gst_bin_remove(pipeline, elem->GetGstElement());
  CHECK(res);
  pipeline_elements_.erase(std::remove(pipeline_elements_.begin(), pipeline_elements_.end(), elem),
                           pipeline_elements_.end());
  delete elem;
  return res;
}

bool IBaseBuilder::ElementLinkRemove(elements::Element* src, elements::Element* dest) {
  gst_element_unlink(src->GetGstElement(), dest->GetGstElement());
  return true;
}

IBaseBuilderObserver* IBaseBuilder::GetObserver() const {
  return observer_;
}

elements::Element* IBaseBuilder::BuildGenericOutput(const OutputUri& output, element_id_t sink_id) {
  elements::Element* sink = CreateSink(output, sink_id);
  pad::Pad* sink_pad = sink->StaticPad("sink");
  if (sink_pad->IsValid()) {
    auto uri = output.GetUrl();
    const auto hlst = output.GetHlsType();
    bool need_push = hlst && *hlst == OutputUri::HLS_PUSH;
    HandleOutputSinkPadCreated(sink_pad, sink_id, uri, need_push);
  }
  delete sink_pad;
  return sink;
}

elements::Element* IBaseBuilder::CreateSink(const OutputUri& output, element_id_t sink_id) {
  IBaseStream* stream = static_cast<IBaseStream*>(GetObserver());
  bool is_cod = stream->GetType() == fastotv::COD_RELAY || stream->GetType() == fastotv::COD_ENCODE;
  elements::Element* sink = elements::sink::build_output(output, sink_id, stream->IsVod(), is_cod);
  return sink;
}

void IBaseBuilder::HandleInputSrcPadCreated(pad::Pad* pad, element_id_t id, const common::uri::GURL& url) {
  if (observer_) {
    observer_->OnInpudSrcPadCreated(pad, id, url);
  }
}

void IBaseBuilder::HandleOutputSinkPadCreated(pad::Pad* pad,
                                              element_id_t id,
                                              const common::uri::GURL& url,
                                              bool need_push) {
  if (observer_) {
    observer_->OnOutputSinkPadCreated(pad, id, url, need_push);
  }
}

bool IBaseBuilder::CreatePipeLine(GstElement** pipeline, elements_line_t* elements) {
  if (!elements) {
    return false;
  }

  if (!InitPipeline()) {
    return false;
  }

  *elements = pipeline_elements_;
  *pipeline = pipeline_;
  return true;
}

}  // namespace stream
}  // namespace fastocloud
