/*  Copyright (C) 2014-2019 FastoGT. All right reserved.
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

#include <gtest/gtest.h>

#include <common/file_system/file_system.h>

#include "stream/link_generator/streamlink.h"

TEST(StreamLinkGenerator, Generate) {
  std::string script_path_str;
  if (!common::file_system::find_file_in_path("streamlink", &script_path_str)) {
    return;
  }

  common::file_system::ascii_file_string_path script_path(script_path_str);
  fastocloud::stream::link_generator::StreamLinkGenerator gena(script_path);
  fastocloud::InputUri out;
  fastocloud::InputUri url(0, common::uri::Url("https://www.youtube.com/watch?v=HVYJJirRADU"));
  url.SetStreamLink(true);
  ASSERT_TRUE(gena.Generate(url, &out));
  ASSERT_FALSE(url.Equals(out));
}
