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

#include "stream/elements/machine_learning/video_ml_filter.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace machine_learning {

class ElementTinyYolov3 : public ElementVideoMLFilter {
 public:
  enum MatrixSize {width=416, height=416};

  typedef ElementsTraits<ELEMENT_TINY_YOLOV3> traits_t;
  typedef ElementVideoMLFilter base_class;

  explicit ElementTinyYolov3(const std::string& name) : base_class(GetPluginName(), name) {}  // allocate element
  ElementTinyYolov3(const std::string& name, GstElement* const element)
      : base_class(GetPluginName(), name, element) {}  // take poiner of element, only wrap

  static std::string GetPluginName() { return traits_t::name(); }
};

}  // namespace machine_learning
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
