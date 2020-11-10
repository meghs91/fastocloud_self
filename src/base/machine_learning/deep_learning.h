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
#include <vector>

#include <common/file_system/path.h>
#include <common/serializer/json_serializer.h>
#include <common/value.h>

#include <fastoml/types.h>

namespace fastocloud {
namespace machine_learning {

struct BackendProperty {
  std::string property;
  std::string value;
};

class DeepLearning : public common::serializer::JsonSerializer<DeepLearning> {
 public:
  typedef common::file_system::ascii_file_string_path file_path_t;
  typedef std::vector<BackendProperty> properties_t;

  DeepLearning();
  DeepLearning(fastoml::SupportedBackends backend,
               const file_path_t& model_path,
               const properties_t& prop = properties_t());

  bool Equals(const DeepLearning& learn) const;

  properties_t GetProperties() const;
  void SetProperties(const properties_t& prop);

  fastoml::SupportedBackends GetBackend() const;
  void SetBackend(fastoml::SupportedBackends backend);

  file_path_t GetModelPath() const;
  void SetModelPath(const file_path_t& path);

  static common::Optional<DeepLearning> MakeDeepLearning(common::HashValue* hash);

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  fastoml::SupportedBackends backend_;
  file_path_t model_path_;
  properties_t properties_;
};

}  // namespace machine_learning
}  // namespace fastocloud
