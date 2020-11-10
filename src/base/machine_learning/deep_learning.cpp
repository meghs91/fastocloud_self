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

#include "base/machine_learning/deep_learning.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>

#include <common/sprintf.h>

#define DEEP_LEARNING_BACKEND_FIELD "backend"
#define DEEP_LEARNING_MODEL_PATH_FIELD "model_path"
#define DEEP_LEARNING_PROPERTIES_FIELD "properties"

namespace fastocloud {
namespace machine_learning {

DeepLearning::DeepLearning() : backend_(fastoml::TENSORFLOW), model_path_(), properties_() {}

DeepLearning::DeepLearning(fastoml::SupportedBackends backend, const file_path_t& model_path, const properties_t& prop)
    : backend_(backend), model_path_(model_path), properties_(prop) {}

DeepLearning::file_path_t DeepLearning::GetModelPath() const {
  return model_path_;
}

void DeepLearning::SetModelPath(const file_path_t& path) {
  model_path_ = path;
}

DeepLearning::properties_t DeepLearning::GetProperties() const {
  return properties_;
}

void DeepLearning::SetProperties(const properties_t& prop) {
  properties_ = prop;
}

fastoml::SupportedBackends DeepLearning::GetBackend() const {
  return backend_;
}

void DeepLearning::SetBackend(fastoml::SupportedBackends backend) {
  backend_ = backend;
}

bool DeepLearning::Equals(const DeepLearning& learn) const {
  return learn.backend_ == backend_ && learn.model_path_ == model_path_;
}

common::Optional<DeepLearning> DeepLearning::MakeDeepLearning(common::HashValue* hash) {
  if (!hash) {
    return common::Optional<DeepLearning>();
  }

  DeepLearning res;
  common::Value* learning_backend_field = hash->Find(DEEP_LEARNING_BACKEND_FIELD);
  int backend;
  if (!learning_backend_field || !learning_backend_field->GetAsInteger(&backend)) {
    return common::Optional<DeepLearning>();
  }
  res.SetBackend(static_cast<fastoml::SupportedBackends>(backend));

  std::string model_path_str;
  common::Value* model_path_field = hash->Find(DEEP_LEARNING_MODEL_PATH_FIELD);
  if (!model_path_field || !model_path_field->GetAsBasicString(&model_path_str)) {
    return common::Optional<DeepLearning>();
  }
  res.SetModelPath(file_path_t(model_path_str));

  common::Value* properties_field = hash->Find(DEEP_LEARNING_PROPERTIES_FIELD);
  common::ArrayValue* arr = nullptr;
  if (properties_field && properties_field->GetAsList(&arr)) {
    properties_t properties;
    for (size_t i = 0; i < arr->GetSize(); ++i) {
      common::Value* prop = nullptr;
      if (arr->Get(i, &prop)) {
        common::HashValue* hprop = nullptr;
        if (prop->GetAsHash(&hprop)) {
          for (auto it = hprop->begin(); it != hprop->end(); ++it) {
            std::string value_str;
            if (it->second->GetAsBasicString(&value_str)) {
              BackendProperty pr = {it->first.as_string(), value_str};
              properties.push_back(pr);
            }
          }
        }
      }
    }
    res.SetProperties(properties);
  }

  return res;
}

common::Error DeepLearning::DoDeSerialize(json_object* serialized) {
  DeepLearning res;
  json_object* jbackend = nullptr;
  json_bool jbackend_exists = json_object_object_get_ex(serialized, DEEP_LEARNING_BACKEND_FIELD, &jbackend);
  if (!jbackend_exists) {
    return common::make_error_inval();
  }
  res.SetBackend(static_cast<fastoml::SupportedBackends>(json_object_get_int(jbackend)));

  json_object* jmodel_path = nullptr;
  json_bool jmodel_path_exists = json_object_object_get_ex(serialized, DEEP_LEARNING_MODEL_PATH_FIELD, &jmodel_path);
  if (!jmodel_path_exists) {
    return common::make_error_inval();
  }
  res.SetModelPath(file_path_t(json_object_get_string(jmodel_path)));

  json_object* jproperties = nullptr;
  json_bool jproperties_exists = json_object_object_get_ex(serialized, DEEP_LEARNING_PROPERTIES_FIELD, &jproperties);
  if (jproperties_exists) {
    properties_t properties;
    size_t len = json_object_array_length(jproperties);
    // [{"input_layer" : "1234"}, {"output_layer" : "321"}]
    for (size_t i = 0; i < len; ++i) {
      json_object* jproperty = json_object_array_get_idx(jproperties, i);
      json_object_object_foreach(jproperty, key, val) { properties.push_back({key, json_object_get_string(val)}); }
    }
    res.SetProperties(properties);
  }

  *this = res;
  return common::Error();
}

common::Error DeepLearning::SerializeFields(json_object* out) const {
  json_object_object_add(out, DEEP_LEARNING_BACKEND_FIELD, json_object_new_int64(backend_));
  const std::string model_path_str = model_path_.GetPath();
  json_object_object_add(out, DEEP_LEARNING_MODEL_PATH_FIELD, json_object_new_string(model_path_str.c_str()));

  json_object* jproperties = json_object_new_array();
  for (size_t i = 0; i < properties_.size(); ++i) {
    json_object* jproperty = json_object_new_object();
    json_object_object_add(jproperty, properties_[i].property.c_str(),
                           json_object_new_string(properties_[i].value.c_str()));
    json_object_array_add(jproperties, jproperty);
  }
  json_object_object_add(out, DEEP_LEARNING_PROPERTIES_FIELD, jproperties);
  return common::Error();
}

}  // namespace machine_learning
}  // namespace fastocloud
