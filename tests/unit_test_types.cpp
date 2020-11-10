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

#include "stream_commands/commands_info/statistic_info.h"
#include "base/constants.h"

TEST(StreamStructInfo, SerializeDeSerialize) {
  fastocloud::StreamInfo sha;
  static const auto test_url = common::uri::Url(TEST_URL);
  sha.id = "test";
  sha.input = {fastocloud::InputUri(0, test_url),
               fastocloud::InputUri(1,test_url),
               fastocloud::InputUri(0,test_url),
               fastocloud::InputUri(3,test_url)};
  sha.output = {fastocloud::OutputUri(4, test_url),
                fastocloud::OutputUri(1, test_url),
                fastocloud::OutputUri(2, test_url),
                fastocloud::OutputUri(3, test_url)};

  time_t start_time = 15;
  time_t lst = 33;
  size_t rest = 1;
  fastocloud::StreamStruct str(sha, start_time, lst, rest);
  fastocloud::StatisticInfo::cpu_load_t cpu_load = 0.33;
  fastocloud::StatisticInfo::rss_t rss = 12;
  time_t time = 10;

  fastocloud::StatisticInfo sinf(str, cpu_load, rss, time);
  json_object* serialized = NULL;
  common::Error err = sinf.Serialize(&serialized);
  ASSERT_FALSE(err);

  fastocloud::StatisticInfo sinf2;
  err = sinf2.DeSerialize(serialized);
  ASSERT_FALSE(err);
  // ASSERT_EQ(sinf.GetStreamStruct(), sinf2.GetStreamStruct());
  ASSERT_EQ(sinf.GetCpuLoad(), sinf2.GetCpuLoad());
  ASSERT_EQ(sinf.GetRssBytes(), sinf2.GetRssBytes());
  ASSERT_EQ(sinf.GetTimestamp(), sinf2.GetTimestamp());

  json_object_put(serialized);
}
