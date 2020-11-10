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

#include "stream/elements/sources/httpsrc.h"

#include <vector>

namespace fastocloud {
namespace stream {
namespace elements {
namespace sources {

void ElementSoupHTTPSrc::SetIsLive(bool live) {
  SetProperty("is-live", live);
}

void ElementSoupHTTPSrc::SetTimeOut(gint secs) {
  SetProperty("timeout", secs);
}

void ElementSoupHTTPSrc::SetUserAgent(const std::string& agent) {
  SetProperty("user-agent", agent);
}

void ElementSoupHTTPSrc::SetProxy(const common::uri::GURL& url) {
  SetProperty("proxy", url.spec());
}

void ElementSoupHTTPSrc::SetProxyID(const std::string& id) {
  SetProperty("proxy-id", id);
}

void ElementSoupHTTPSrc::SetProxyPW(const std::string& pw) {
  SetProperty("proxy-pw", pw);
}

void ElementSoupHTTPSrc::SetAutomaticRedirect(bool redirect) {
  SetProperty("automatic-redirect", redirect);
}

void ElementSoupHTTPSrc::SetCookies(const std::vector<std::string>& cookies) {
  size_t number_of_row = cookies.size();
  const char** gcookies = (const char**)malloc(sizeof(char*) * number_of_row + 1);
  for (size_t i = 0; i < number_of_row; ++i) {
    gcookies[i] = cookies[i].c_str();
  }
  gcookies[number_of_row] = NULL;
  SetProperty("cookies", gcookies);
  free(gcookies);
}

ElementSoupHTTPSrc* make_http_src(const std::string& location,
                                  const common::Optional<std::string>& user_agent,
                                  common::Optional<common::uri::GURL> proxy,
                                  gint timeout_secs,
                                  element_id_t input_id) {
  ElementSoupHTTPSrc* http_src = make_sources<ElementSoupHTTPSrc>(input_id);
  http_src->SetLocation(location);
  http_src->SetTimeOut(timeout_secs);
  http_src->SetIsLive(true);
  if (user_agent) {
    http_src->SetUserAgent(*user_agent);
  }
  if (proxy) {
    http_src->SetProxy(*proxy);
    http_src->SetAutomaticRedirect(false);
    if (proxy->has_username()) {
      http_src->SetProxyID(proxy->username());
    }
    if (proxy->has_password()) {
      http_src->SetProxyPW(proxy->password());
    }
  }
  return http_src;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
