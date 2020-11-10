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

#include "stream/dumpers/htmldump.h"

#include <gst/gst.h>

#include <fstream>
#include <string>

#include "stream/elements/element.h"

#define JS_LIB "https://github.com/mdaines/viz.js/releases/download/v1.8.2/viz-lite.js"

namespace fastocloud {
namespace stream {
namespace dumper {

bool HtmlDump::Dump(GstBin* pipeline, const common::file_system::ascii_file_string_path& path) {
  if (!path.IsValid()) {
    return false;
  }

  if (!pipeline) {
    return false;
  }

#if !defined(GST_DISABLE_GST_DEBUG)
  std::ofstream dumpfile(path.GetPath());
  if (!dumpfile.is_open()) {
    return false;
  }

#if GST_CHECK_VERSION(1, 11, 1)
  char* dot_description = gst_debug_bin_to_dot_data(pipeline, GST_DEBUG_GRAPH_SHOW_ALL);
  if (!dot_description) {
    return false;
  }

  std::string pipeline_description(dot_description);
  dumpfile << "<html><head></head>"
           << "<body>"
           << "  <script type=\"text/javascript\" src=\"" JS_LIB "\"></script>"
           << "  <script>"
           << "    document.body.innerHTML += Viz(String.raw`" << pipeline_description << "`);"
           << "  </script>"
           << "</body>"
           << "</html>";
  g_free(dot_description);
  return true;
#else
  return false;
#endif
#else
  return false;
#endif
}

}  // namespace dumper
}  // namespace stream
}  // namespace fastocloud
