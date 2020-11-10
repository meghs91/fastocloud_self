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

#include "stream/config.h"

namespace fastocloud {
namespace stream {

Config::Config(fastotv::StreamType type, size_t max_restart_attempts, const input_t& input, const output_t& output)
    : type_(type), max_restart_attempts_(max_restart_attempts), ttl_sec_(), input_(input), output_(output) {}

Config::~Config() {}

fastotv::StreamType Config::GetType() const {
  return type_;
}

input_t Config::GetUrl() const {
  return input_;
}

void Config::SetInput(const input_t& input) {
  input_ = input;
}

output_t Config::GetOutput() const {
  return output_;
}

void Config::SetOutput(const output_t& output) {
  output_ = output;
}

size_t Config::GetMaxRestartAttempts() const {
  return max_restart_attempts_;
}

void Config::SetMaxRestartAttempts(size_t attempts) {
  max_restart_attempts_ = attempts;
}

Config::ttl_t Config::GetTimeToLifeStream() const {
  return ttl_sec_;
}

void Config::SetTimeToLifeStream(ttl_t ttl) {
  ttl_sec_ = ttl;
}

Config* Config::Clone() const {
  return new Config(*this);
}

}  // namespace stream
}  // namespace fastocloud
