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

#include <common/file_system/file_system.h>
#include <common/file_system/path.h>
#include <common/uri/gurl.h>

#include "stream/elements/element.h"  // for ElementEx, SupportedElements::ELEMENT_...
#include "stream/elements/sink/sink.h"
#include "stream/stypes.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

struct HlsOutput {
  std::string location;
  std::string play_locataion;
  std::string playlist_root;
  uint32_t paylist_length;
  uint32_t max_files;
};

common::Error MakeHlsOutput(const common::uri::GURL& uri,
                            const common::Optional<common::file_system::ascii_directory_string_path>& http_root,
                            HlsOutput* out);
common::Error MakeVodHlsOutput(const common::uri::GURL& uri,
                               const common::Optional<common::file_system::ascii_directory_string_path>& http_root,
                               HlsOutput* out);

class ElementHLSSink : public ElementBinEx<ELEMENT_HLS_SINK> {
 public:
  typedef ElementBinEx<ELEMENT_HLS_SINK> base_class;
  using base_class::base_class;

  void SetLocation(const std::string& location = "segment%05d.ts");  // String; Default: "segment%05d.ts"
  std::string GetLocation() const;

  void SetPlayLocation(const std::string& play_location = "playlist.m3u8");  // String; Default: "playlist.m3u8"
  void SetTargetDuration(guint duration = 15);                               // Range: 0 - 4294967295 Default: 15
  void SetPlayListRoot(const std::string& play_root);                        // String, Default: null
  void SetPlaylistLenght(guint duration = 5);                                // Range: 0 - 4294967295 Default: 5
  void SetMaxFiles(guint max_files = 10);                                    // Range: 0 - 4294967295 Default: 10
};

class ElementSoupHttpSink : public ElementEx<ELEMENT_SOUP_HTTP_CLIENT_SINK> {
 public:
  typedef ElementEx<ELEMENT_SOUP_HTTP_CLIENT_SINK> base_class;
  using base_class::base_class;

  void SetLocation(const std::string& location);  // String; Default: null
};

ElementSoupHttpSink* make_http_soup_sink(element_id_t sink_id, const std::string& location);
ElementHLSSink* make_http_sink(element_id_t sink_id, const HlsOutput& output, guint ts_duration);

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
