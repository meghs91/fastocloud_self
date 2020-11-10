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

#include "stream/streams/test/test_life_stream.h"

#include "stream/streams/builders/test/test_life_stream_builder.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace test {

TestLifeStream::TestLifeStream(const RelayConfig* config, IStreamClient* client, StreamStruct* stats)
    : RelayStream(config, client, stats) {}

const char* TestLifeStream::ClassName() const {
  return "TestLifeStream";
}

IBaseBuilder* TestLifeStream::CreateBuilder() {
  const RelayConfig* econf = static_cast<const RelayConfig*>(GetConfig());
  return new builders::test::TestLifeStreamBuilder(econf, this);
}

}  // namespace test
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
