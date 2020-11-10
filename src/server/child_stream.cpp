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

#include "server/child_stream.h"

#include <common/file_system/file_system.h>

namespace fastocloud {
namespace server {

ChildStream::ChildStream(common::libev::IoLoop* server, const StreamInfo& conf) : base_class(server), conf_(conf) {}

fastotv::stream_id_t ChildStream::GetStreamID() const {
  return conf_.id;
}

void ChildStream::CleanUp() {
  if (conf_.type == fastotv::VOD_ENCODE || conf_.type == fastotv::VOD_RELAY || conf_.type == fastotv::CATCHUP ||
      conf_.type == fastotv::TIMESHIFT_RECORDER || conf_.type == fastotv::TEST_LIFE || conf_.type == fastotv::SCREEN) {
    return;
  }

  for (auto out_uri : conf_.output) {
    auto ouri = out_uri.GetUrl();
    if (ouri.SchemeIsHTTPOrHTTPS()) {
      const auto http_root = out_uri.GetHttpRoot();
      if (http_root) {
        common::file_system::remove_directory(http_root->GetPath(), true);
      }
    }
  }
}

}  // namespace server
}  // namespace fastocloud
