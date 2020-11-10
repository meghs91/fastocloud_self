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

#include "stream/elements/sources/sources.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

class ElementAppSrc : public ElementBaseSrc<ELEMENT_APP_SRC> {
 public:
  typedef ElementBaseSrc<ELEMENT_APP_SRC> base_class;
  typedef void (*need_data_callback_t)(GstElement* pipeline, guint size, gpointer user_data);
  using base_class::base_class;

  gboolean RegisterNeedDataCallback(need_data_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;

  GstFlowReturn PushBuffer(GstBuffer* buffer);
  void SendEOS();
};

ElementAppSrc* make_app_src(element_id_t input_id);

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
