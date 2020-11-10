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

#include "base/amazon_kinesis/amazon_kinesis.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>

#define AMAZON_KINESIS_STREAM_NAME_FIELD "stream_name"
#define AMAZON_KINESIS_SECRET_KEY_FIELD "secret_key"
#define AMAZON_KINESIS_ACCESS_KEY_FIELD "access_key"

namespace fastocloud {
namespace amazon_kinesis {

AmazonKinesis::AmazonKinesis() : AmazonKinesis(std::string(), std::string(), std::string()) {}

AmazonKinesis::AmazonKinesis(const std::string& stream_name,
                             const std::string& secret_key,
                             const std::string& access_key)
    : stream_name_(stream_name), secret_key_(secret_key), access_key_(access_key) {}

bool AmazonKinesis::Equals(const AmazonKinesis& aws) const {
  return aws.stream_name_ == stream_name_ && aws.secret_key_ == secret_key_ && aws.access_key_ == access_key_;
}

common::Optional<AmazonKinesis> AmazonKinesis::MakeAmazonKinesis(common::HashValue* hash) {
  if (!hash) {
    return common::Optional<AmazonKinesis>();
  }

  AmazonKinesis res;
  common::Value* stream_name_field = hash->Find(AMAZON_KINESIS_STREAM_NAME_FIELD);
  std::string stream_name;
  if (stream_name_field && stream_name_field->GetAsBasicString(&stream_name)) {
    res.stream_name_ = stream_name;
  }

  common::Value* secret_key_field = hash->Find(AMAZON_KINESIS_SECRET_KEY_FIELD);
  std::string secret_key;
  if (secret_key_field && secret_key_field->GetAsBasicString(&secret_key)) {
    res.secret_key_ = secret_key;
  }

  common::Value* access_key_field = hash->Find(AMAZON_KINESIS_ACCESS_KEY_FIELD);
  std::string access_key;
  if (access_key_field && access_key_field->GetAsBasicString(&access_key)) {
    res.access_key_ = access_key;
  }

  return res;
}

common::Error AmazonKinesis::DoDeSerialize(json_object* serialized) {
  AmazonKinesis res;
  json_object* jstream_name = nullptr;
  json_bool jstream_name_exists =
      json_object_object_get_ex(serialized, AMAZON_KINESIS_STREAM_NAME_FIELD, &jstream_name);
  if (jstream_name_exists) {
    res.stream_name_ = json_object_get_string(jstream_name);
  }

  json_object* jsecret_key = nullptr;
  json_bool jsecret_key_exists = json_object_object_get_ex(serialized, AMAZON_KINESIS_SECRET_KEY_FIELD, &jsecret_key);
  if (jsecret_key_exists) {
    res.secret_key_ = json_object_get_string(jsecret_key);
  }

  json_object* jaccess_key = nullptr;
  json_bool jaccess_key_exists = json_object_object_get_ex(serialized, AMAZON_KINESIS_ACCESS_KEY_FIELD, &jaccess_key);
  if (jaccess_key_exists) {
    res.access_key_ = json_object_get_string(jaccess_key);
  }

  *this = res;
  return common::Error();
}

common::Error AmazonKinesis::SerializeFields(json_object* out) const {
  json_object_object_add(out, AMAZON_KINESIS_STREAM_NAME_FIELD, json_object_new_string(stream_name_.c_str()));
  json_object_object_add(out, AMAZON_KINESIS_SECRET_KEY_FIELD, json_object_new_string(secret_key_.c_str()));
  json_object_object_add(out, AMAZON_KINESIS_ACCESS_KEY_FIELD, json_object_new_string(access_key_.c_str()));
  return common::Error();
}

}  // namespace amazon_kinesis
}  // namespace fastocloud
