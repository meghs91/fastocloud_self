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

#include "gtest/gtest.h"

#include "base/config_fields.h"
#include "base/constants.h"
#include "base/stream_config_parse.h"

#include "server/options/options.h"

namespace {
const char kTimeshiftRecorderConfig[] = R"({
    "id" : "test_1",
    "input" : {"urls" : [ {"id" : 1, "uri" : "http://example.com/manager/fo/forward2.php?cid=14"} ]},
    "output" : {"urls" : [ {"id" : 80, "timeshift_dir" : "/var/www/html/live/14"} ]},
    "type" : 3
  })";
}

TEST(Options, logo_path) {
  std::string cfg = "{\"" LOGO_FIELD "\" : {\"path\": \"file:///home/user/logo.png\"}}";
  fastocloud::StreamConfig args = fastocloud::MakeConfigFromJson(cfg);
  ASSERT_TRUE(args);
  common::ErrnoError err = fastocloud::server::options::ValidateConfig(args);
  ASSERT_FALSE(err);
  ASSERT_EQ(args->GetSize(), 1);

  cfg = "{\"" LOGO_FIELD "\" : {\"path\": \"http://home/user/logo.png\"}}";
  args = fastocloud::MakeConfigFromJson(cfg);
  ASSERT_TRUE(args);
  err = fastocloud::server::options::ValidateConfig(args);
  ASSERT_FALSE(err);
  ASSERT_EQ(args->GetSize(), 1);
}

TEST(Options, cfgs) {
  fastocloud::StreamConfig args = fastocloud::MakeConfigFromJson(kTimeshiftRecorderConfig);
  ASSERT_TRUE(args);

  common::ErrnoError err = fastocloud::server::options::ValidateConfig(args);
  ASSERT_FALSE(err);
  ASSERT_EQ(args->GetSize(), 4);
}
