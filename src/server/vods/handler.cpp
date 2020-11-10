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

#include "server/vods/handler.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <utility>

#include "server/base/ihttp_requests_observer.h"
#include "server/vods/client.h"

namespace fastocloud {
namespace server {

VodsHandler::VodsHandler(base::IHttpRequestsObserver* observer)
    : base_class(), http_root_(http_directory_path_t::MakeHomeDir()), observer_(observer) {}

void VodsHandler::SetHttpRoot(const http_directory_path_t& http_root) {
  http_root_ = http_root;
}

void VodsHandler::PreLooped(common::libev::IoLoop* server) {
  UNUSED(server);
}

void VodsHandler::Accepted(common::libev::IoClient* client) {
  base_class::Accepted(client);
}

void VodsHandler::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  base_class::Moved(server, client);
}

void VodsHandler::Closed(common::libev::IoClient* client) {
  base_class::Closed(client);
}

void VodsHandler::TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) {
  UNUSED(server);
  UNUSED(id);
}

void VodsHandler::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void VodsHandler::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void VodsHandler::ChildStatusChanged(common::libev::IoChild* child, int status, int signal) {
  UNUSED(child);
  UNUSED(status);
  UNUSED(signal);
}

void VodsHandler::DataReceived(common::libev::IoClient* client) {
  char buff[BUF_SIZE] = {0};
  size_t nread = 0;
  common::ErrnoError errn = client->SingleRead(buff, BUF_SIZE - 1, &nread);
  if (errn || nread == 0) {
    ignore_result(client->Close());
    delete client;
    return;
  }

  VodsClient* hclient = static_cast<server::VodsClient*>(client);
  ProcessReceived(hclient, buff, nread);
}

void VodsHandler::DataReadyToWrite(common::libev::IoClient* client) {
  UNUSED(client);
}

void VodsHandler::PostLooped(common::libev::IoLoop* server) {
  UNUSED(server);
}

void VodsHandler::ProcessReceived(VodsClient* hclient, const char* request, size_t req_len) {
  static const common::libev::http::HttpServerInfo hinf(PROJECT_NAME_TITLE, PROJECT_DOMAIN);
  common::http::HttpRequest hrequest;
  std::string request_str(request, req_len);
  DEBUG_LOG() << "Http request:\n" << request_str;

  std::pair<common::http::http_status, common::Error> result = common::http::parse_http_request(request_str, &hrequest);
  common::http::headers_t extra_headers = {{"Access-Control-Allow-Origin", "*"}};
  if (result.second) {
    const std::string error_text = result.second->GetDescription();
    DEBUG_MSG_ERROR(result.second, common::logging::LOG_LEVEL_ERR);
    common::ErrnoError err =
        hclient->SendError(common::http::HP_1_1, result.first, extra_headers, error_text.c_str(), false, hinf);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    ignore_result(hclient->Close());
    delete hclient;
    return;
  }

  // keep alive
  common::http::header_t connection_field;
  bool is_find_connection = hrequest.FindHeaderByKey("Connection", false, &connection_field);
  bool IsKeepAlive = is_find_connection ? common::EqualsASCII(connection_field.value, "Keep-Alive", false) : false;
  const common::http::http_protocol protocol = hrequest.GetProtocol();
  if (hrequest.GetMethod() == common::http::http_method::HM_GET ||
      hrequest.GetMethod() == common::http::http_method::HM_HEAD) {
    auto url = hrequest.GetURL();
    if (!url.is_valid()) {  // for hls
      common::ErrnoError err =
          hclient->SendError(protocol, common::http::HS_NOT_FOUND, extra_headers, "File not found.", IsKeepAlive, hinf);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      }
      goto finish;
    }

    const std::string path_abs = url.PathForRequest();
    auto file_path = http_root_.MakeConcatFileStringPath(path_abs.substr(1));
    if (!file_path) {
      common::ErrnoError err =
          hclient->SendError(protocol, common::http::HS_NOT_FOUND, extra_headers, "File not found.", IsKeepAlive, hinf);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      }
      goto finish;
    }

    common::http::http_status recommend_status = common::http::HS_OK;
    if (observer_) {
      observer_->OnHttpRequest(hclient, *file_path, &recommend_status);
    }

    if (recommend_status == common::http::HS_FORBIDDEN) {
      common::ErrnoError err =
          hclient->SendError(protocol, common::http::HS_FORBIDDEN, extra_headers, "Rejected.", IsKeepAlive, hinf);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      }
      goto finish;
    }

    const std::string file_path_str = file_path->GetPath();
    int open_flags = O_RDONLY;
    struct stat sb;
    if (stat(file_path_str.c_str(), &sb) < 0) {
      if (recommend_status == common::http::HS_ACCEPTED) {
        extra_headers.push_back({"Location", url.spec()});
        common::ErrnoError err = hclient->SendError(protocol, common::http::HS_ACCEPTED, extra_headers,
                                                    "Request in progress.", IsKeepAlive, hinf);
        if (err) {
          DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
        }
        goto finish;
      }

      common::ErrnoError err =
          hclient->SendError(protocol, common::http::HS_NOT_FOUND, extra_headers, "File not found.", IsKeepAlive, hinf);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      }
      goto finish;
    }

    if (S_ISDIR(sb.st_mode)) {
      common::ErrnoError err =
          hclient->SendError(protocol, common::http::HS_BAD_REQUEST, extra_headers, "Bad filename.", IsKeepAlive, hinf);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      }
      goto finish;
    }

    int file = open(file_path_str.c_str(), open_flags);
    if (file == INVALID_DESCRIPTOR) { /* open the file for reading */
      common::ErrnoError err = hclient->SendError(protocol, common::http::HS_FORBIDDEN, extra_headers,
                                                  "File is protected.", IsKeepAlive, hinf);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      }
      goto finish;
    }

    const std::string fileName = url.ExtractFileName();
    const char* mime = common::http::MimeTypes::GetType(fileName.c_str());
    common::ErrnoError err = hclient->SendHeaders(protocol, common::http::HS_OK, extra_headers, mime, &sb.st_size,
                                                  &sb.st_mtime, IsKeepAlive, hinf);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      ::close(file);
      goto finish;
    }

    if (hrequest.GetMethod() == common::http::http_method::HM_GET) {
      common::ErrnoError err = hclient->SendFileByFd(protocol, file, sb.st_size);
      if (err) {
        DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      } else {
        DEBUG_LOG() << "Sent file path: " << file_path_str << ", size: " << sb.st_size;
      }
    }

    ::close(file);
  }

finish:
  if (!IsKeepAlive) {
    ignore_result(hclient->Close());
    delete hclient;
  }
}

}  // namespace server
}  // namespace fastocloud
