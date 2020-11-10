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

#include <string>

#include <gst/gstelement.h>

#include "stream/config.h"
#include "stream/gst_types.h"
#include "stream/ilinker.h"
#include "stream/stypes.h"

namespace fastocloud {
namespace stream {

class IBaseBuilderObserver;

namespace pad {
class Pad;
}

class IBaseBuilder : public ILinker {
 public:
  IBaseBuilder(const Config* config, IBaseBuilderObserver* observer);
  ~IBaseBuilder() override;

  const Config* GetConfig() const;

  bool CreatePipeLine(GstElement** pipeline, elements_line_t* elements) WARN_UNUSED_RESULT;

  elements::Element* GetElementByName(const std::string& name) const;

  bool ElementAdd(elements::Element* elem) override;
  bool ElementLink(elements::Element* src, elements::Element* dest) override;
  bool ElementRemove(elements::Element* elem) override;
  bool ElementLinkRemove(elements::Element* src, elements::Element* dest) override;

 protected:
  IBaseBuilderObserver* GetObserver() const;

  elements::Element* BuildGenericOutput(const OutputUri& output, element_id_t sink_id);
  virtual elements::Element* CreateSink(const OutputUri& output, element_id_t sink_id);

  virtual bool InitPipeline() WARN_UNUSED_RESULT = 0;

  void HandleInputSrcPadCreated(pad::Pad* pad, element_id_t id, const common::uri::GURL& url);
  void HandleOutputSinkPadCreated(pad::Pad* pad, element_id_t id, const common::uri::GURL& url, bool need_push);

 private:
  const Config* const config_;
  IBaseBuilderObserver* const observer_;
  GstElement* const pipeline_;
  elements_line_t pipeline_elements_;
};

}  // namespace stream
}  // namespace fastocloud
