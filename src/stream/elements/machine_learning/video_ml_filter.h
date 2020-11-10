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
#include <utility>
#include <vector>

#include <common/sprintf.h>

#include "stream/elements/element.h"

typedef struct _GstBackend GstBackend;

namespace fastocloud {
namespace stream {
namespace elements {
namespace machine_learning {

class ElementVideoMLFilter : public Element {
 public:
  typedef Element base_class;
  typedef void (*new_prediction_callback_t)(GstElement* elem, gpointer meta, gpointer user_data);
  using base_class::base_class;

  void SetBackend(GstBackend* backend);

  gboolean RegisterNewPredictionCallback(new_prediction_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
};

}  // namespace machine_learning
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
