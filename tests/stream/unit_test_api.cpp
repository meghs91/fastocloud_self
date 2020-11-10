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
#include "base/inputs_outputs.h"

#include "stream/configs_factory.h"
#include "stream/stypes.h"

TEST(Api, init) {
  fastocloud::StreamConfig emp;
  fastocloud::stream::Config* empty_api = nullptr;
  common::Error err = fastocloud::stream::make_config(emp, &empty_api);
  ASSERT_TRUE(err);
  fastocloud::output_t ouri;
  fastocloud::OutputUri uri;
  uri.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri);

  ASSERT_TRUE(fastocloud::IsTestInputUrl(fastocloud::InputUri(0, common::uri::Url(TEST_URL))));
  ASSERT_TRUE(fastocloud::stream::IsScreenUrl(common::uri::Url(SCREEN_URL)));
  ASSERT_TRUE(fastocloud::stream::IsRecordingUrl(common::uri::Url(RECORDING_URL)));
}

TEST(Api, logo) {
  fastocloud::output_t ouri;
  fastocloud::OutputUri uri2;
  uri2.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri2);
}
