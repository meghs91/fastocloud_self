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

#include <common/serializer/json_serializer.h>
#include <common/value.h>

namespace fastocloud {
namespace amazon_kinesis {

class AmazonKinesis : public common::serializer::JsonSerializer<AmazonKinesis> {
 public:
  AmazonKinesis();
  AmazonKinesis(const std::string& stream_name, const std::string& secret_key, const std::string& access_key);

  bool Equals(const AmazonKinesis& aws) const;

  static common::Optional<AmazonKinesis> MakeAmazonKinesis(common::HashValue* hash);

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  std::string stream_name_;
  std::string secret_key_;
  std::string access_key_;
};

}  // namespace amazon_kinesis
}  // namespace fastocloud
