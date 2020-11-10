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

#include <common/file_system/path.h>
#include <common/libev/http/http_client.h>

namespace fastocloud {
namespace server {
namespace base {

class IHttpRequestsObserver {
 public:
  typedef common::file_system::ascii_file_string_path file_path_t;

  virtual void OnHttpRequest(common::libev::http::HttpClient* client,
                             const file_path_t& file,
                             common::http::http_status* recommend_status) = 0;
  virtual ~IHttpRequestsObserver();
};

}  // namespace base
}  // namespace server
}  // namespace fastocloud
