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

#include "stream_commands/commands_info/changed_sources_info.h"

#include <string>

#define CHANGE_SOURCES_ID_FIELD "id"
#define CHANGE_SOURCES_URL_FIELD "url"

namespace fastocloud {

ChangedSouresInfo::ChangedSouresInfo(const fastotv::stream_id_t& sid, const url_t& url)
    : base_class(), id_(sid), url_(url) {}

ChangedSouresInfo::ChangedSouresInfo() : base_class(), id_(), url_() {}

common::Error ChangedSouresInfo::SerializeFields(json_object* out) const {
  json_object* url_obj = nullptr;
  common::Error err = url_.Serialize(&url_obj);
  if (err) {
    return err;
  }

  ignore_result(SetStringField(out, CHANGE_SOURCES_ID_FIELD, id_));
  ignore_result(SetObjectField(out, CHANGE_SOURCES_URL_FIELD, url_obj));
  return common::Error();
}

ChangedSouresInfo::url_t ChangedSouresInfo::GetUrl() const {
  return url_;
}

fastotv::stream_id_t ChangedSouresInfo::GetStreamID() const {
  return id_;
}

common::Error ChangedSouresInfo::DoDeSerialize(json_object* serialized) {
  fastotv::stream_id_t sid;
  common::Error err = GetStringField(serialized, CHANGE_SOURCES_ID_FIELD, &sid);
  if (err) {
    return err;
  }

  json_object* jurl = nullptr;
  err = GetObjectField(serialized, CHANGE_SOURCES_URL_FIELD, &jurl);
  if (err) {
    return err;
  }

  url_t url;
  err = url.DeSerialize(jurl);
  if (err) {
    return err;
  }

  *this = ChangedSouresInfo(sid, url);
  return common::Error();
}

}  // namespace fastocloud
