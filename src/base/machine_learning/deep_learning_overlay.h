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

class DeepLearningOverlay : public common::serializer::JsonSerializer<DeepLearningOverlay> {
 public:
  typedef common::file_system::ascii_file_string_path file_path_t;

  DeepLearningOverlay();
  explicit DeepLearningOverlay(const file_path_t& labels_path);

  bool Equals(const DeepLearningOverlay& learn) const;

  file_path_t GetLabelsPath() const;
  void SetLabelsPath(const file_path_t& path);

  static common::Optional<DeepLearningOverlay> MakeDeepLearningOverlay(common::HashValue* hash);

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  file_path_t labels_path_;
};

}  // namespace machine_learning
}  // namespace fastocloud
