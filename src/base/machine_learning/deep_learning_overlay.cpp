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

#include "base/machine_learning/deep_learning_overlay.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <common/sprintf.h>

#define DEEP_LEARNING_LABELS_PATH_FIELD "labels_path"

namespace fastocloud {
namespace machine_learning {

DeepLearningOverlay::DeepLearningOverlay() : labels_path_() {}

DeepLearningOverlay::DeepLearningOverlay(const file_path_t& labels_path) : labels_path_(labels_path) {}

DeepLearningOverlay::file_path_t DeepLearningOverlay::GetLabelsPath() const {
  return labels_path_;
}

void DeepLearningOverlay::SetLabelsPath(const file_path_t& path) {
  labels_path_ = path;
}

bool DeepLearningOverlay::Equals(const DeepLearningOverlay& learn) const {
  return learn.labels_path_ == labels_path_;
}

common::Optional<DeepLearningOverlay> DeepLearningOverlay::MakeDeepLearningOverlay(common::HashValue* hash) {
  if (!hash) {
    return common::Optional<DeepLearningOverlay>();
  }

  DeepLearningOverlay res;
  common::Value* paths_field = hash->Find(DEEP_LEARNING_LABELS_PATH_FIELD);
  std::string paths;
  if (!paths_field || !paths_field->GetAsBasicString(&paths)) {
    return common::Optional<DeepLearningOverlay>();
  }
  res.SetLabelsPath(file_path_t(paths));
  return res;
}

common::Error DeepLearningOverlay::DoDeSerialize(json_object* serialized) {
  DeepLearningOverlay res;
  json_object* jlabels_path = nullptr;
  json_bool jlabels_path_exists = json_object_object_get_ex(serialized, DEEP_LEARNING_LABELS_PATH_FIELD, &jlabels_path);
  if (!jlabels_path_exists) {
    return common::make_error_inval();
  }
  res.SetLabelsPath(file_path_t(json_object_get_string(jlabels_path)));

  *this = res;
  return common::Error();
}

common::Error DeepLearningOverlay::SerializeFields(json_object* out) const {
  const std::string labels_path_str = labels_path_.GetPath();
  json_object_object_add(out, DEEP_LEARNING_LABELS_PATH_FIELD, json_object_new_string(labels_path_str.c_str()));
  return common::Error();
}

}  // namespace machine_learning
}  // namespace fastocloud
