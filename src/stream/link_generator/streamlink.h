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

#include "stream/link_generator/ilink_generator.h"

namespace fastocloud {
namespace stream {
namespace link_generator {

class StreamLinkGenerator : public ILinkGenerator {
 public:
  typedef common::Optional<common::uri::GURL> http_proxy_t;
  explicit StreamLinkGenerator(const common::file_system::ascii_file_string_path& script_path);

  bool Generate(const InputUri& src, InputUri* out) const override WARN_UNUSED_RESULT;

 private:
  const common::file_system::ascii_file_string_path script_path_;
};

}  // namespace link_generator
}  // namespace stream
}  // namespace fastocloud
