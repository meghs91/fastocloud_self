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

#include "stream/stypes.h"

TEST(element_id_t, GetElementId) {
  fastocloud::stream::element_id_t id;
  ASSERT_FALSE(fastocloud::stream::GetElementId("udv_", nullptr));
  ASSERT_FALSE(fastocloud::stream::GetElementId("udv_", &id));
  ASSERT_FALSE(fastocloud::stream::GetElementId("udv_42_udv", &id));
  ASSERT_FALSE(fastocloud::stream::GetElementId("udv42", &id));
  ASSERT_FALSE(fastocloud::stream::GetElementId("_42", &id) && id == 42);
  ASSERT_TRUE(fastocloud::stream::GetElementId("udv_0", &id) && id == 0);
  ASSERT_TRUE(fastocloud::stream::GetElementId("udv_1", &id) && id == 1);
  ASSERT_TRUE(fastocloud::stream::GetElementId("udv_777", &id) && id == 777);
  ASSERT_TRUE(fastocloud::stream::GetElementId("udv_udv_777", &id) && id == 777);
  ASSERT_TRUE(fastocloud::stream::GetElementId("udv_udv1_udv_777", &id) && id == 777);
  ASSERT_TRUE(fastocloud::stream::GetElementId("queue2_776", &id) && id == 776);
  ASSERT_TRUE(fastocloud::stream::GetElementId("udv_24_udv_42", &id) && id == 42);
}

TEST(element_id_t, GetPadId) {
  int id;
  ASSERT_FALSE(fastocloud::stream::GetPadId("udv_", nullptr));
  ASSERT_FALSE(fastocloud::stream::GetPadId("udv_", &id));
  ASSERT_FALSE(fastocloud::stream::GetPadId("udv_42_udv", &id));
  ASSERT_FALSE(fastocloud::stream::GetPadId("udv42", &id));
  ASSERT_FALSE(fastocloud::stream::GetPadId("_42", &id) && id == 42);
  ASSERT_TRUE(fastocloud::stream::GetPadId("udv_0", &id) && id == 0);
  ASSERT_TRUE(fastocloud::stream::GetPadId("udv_1", &id) && id == 1);
  ASSERT_TRUE(fastocloud::stream::GetPadId("udv_777", &id) && id == 777);
  ASSERT_TRUE(fastocloud::stream::GetPadId("udv_udv_777", &id) && id == 777);
  ASSERT_TRUE(fastocloud::stream::GetPadId("udv_udv1_udv_777", &id) && id == 777);
  ASSERT_TRUE(fastocloud::stream::GetPadId("queue2_776", &id) && id == 776);
  ASSERT_TRUE(fastocloud::stream::GetPadId("udv_24_udv_42", &id) && id == 42);
}

TEST(m3u8, GetIndexFromHttpTsTemplate) {
  uint64_t ind;
  ASSERT_TRUE(fastocloud::stream::GetIndexFromHttpTsTemplate("123_324.ts", &ind));
  ASSERT_EQ(ind, 324);

  uint64_t ind2;
  ASSERT_TRUE(fastocloud::stream::GetIndexFromHttpTsTemplate("123_0.ts", &ind2));
  ASSERT_EQ(ind2, 0);

  uint64_t ind3;
  ASSERT_FALSE(fastocloud::stream::GetIndexFromHttpTsTemplate("123_g.ts", &ind3));
}
