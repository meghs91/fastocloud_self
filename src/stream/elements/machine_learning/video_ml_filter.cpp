/*  Copyright (C) 2014-2019 FastoGT. All right reserved.
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

#include "stream/elements/machine_learning/video_ml_filter.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace machine_learning {

void ElementVideoMLFilter::SetBackend(GstBackend* backend) {
  SetProperty("backend", backend);
}

gboolean ElementVideoMLFilter::RegisterNewPredictionCallback(new_prediction_callback_t cb, gpointer user_data) {
  return RegisterCallback("new-prediction", G_CALLBACK(cb), user_data);
}

}  // namespace machine_learning
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
