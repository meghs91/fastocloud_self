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

#include <string>

#include <common/macros.h>

#include "base/inputs_outputs.h"

namespace fastocloud {
namespace stream {

class Config : public common::ClonableBase<Config> {
 public:
  enum { report_delay_sec = 10 };
  typedef common::Optional<time_t> ttl_t;
  Config(fastotv::StreamType type, size_t max_restart_attempts, const input_t& input, const output_t& output);
  virtual ~Config();

  fastotv::StreamType GetType() const;

  input_t GetUrl() const;  // all except timeshift_play
  void SetInput(const input_t& input);

  output_t GetOutput() const;  // all except timeshift_rec
  void SetOutput(const output_t& output);

  size_t GetMaxRestartAttempts() const;
  void SetMaxRestartAttempts(size_t attempts);

  ttl_t GetTimeToLifeStream() const;
  void SetTimeToLifeStream(ttl_t ttl);

  Config* Clone() const override;

 private:
  fastotv::StreamType type_;
  size_t max_restart_attempts_;
  ttl_t ttl_sec_;

  input_t input_;
  output_t output_;
};

}  // namespace stream
}  // namespace fastocloud
