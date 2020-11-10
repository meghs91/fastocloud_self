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

#include "stream/dumpers/dumpers_factory.h"

#include <string>

#include <common/string_util.h>

#include "stream/dumpers/htmldump.h"

#define HTML_FORMAT "html"

namespace fastocloud {
namespace stream {
namespace dumper {

IDumper* DumpersFactory::CreateDumper(const common::file_system::ascii_file_string_path& path) {
  if (!path.IsValid()) {
    return nullptr;
  }

  std::string ext = path.GetExtension();
  if (common::EqualsASCII(ext, HTML_FORMAT, false)) {
    return new HtmlDump;
  }

  return new HtmlDump;
}

}  // namespace dumper
}  // namespace stream
}  // namespace fastocloud
