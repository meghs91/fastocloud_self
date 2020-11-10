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

#include "stream/streams/vod/vod_encoding_stream.h"

#include "base/utils.h"

namespace fastocloud {
namespace stream {
namespace streams {

VodEncodeStream::VodEncodeStream(const VodEncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : base_class(config, client, stats) {}

const char* VodEncodeStream::ClassName() const {
  return "VodEncodeStream";
}

void VodEncodeStream::PostExecCleanup() {
  const VodEncodeConfig* vconfig = static_cast<const VodEncodeConfig*>(GetConfig());
  if (vconfig->GetCleanupTS()) {
    for (const OutputUri& output : vconfig->GetOutput()) {
      common::uri::GURL uri = output.GetUrl();
      if (uri.SchemeIsHTTPOrHTTPS()) {
        const auto http_path = output.GetHttpRoot();
        if (http_path) {
          RemoveFilesByExtension(*http_path, CHUNK_EXT);
        }
      }
    }
  }
}

}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
