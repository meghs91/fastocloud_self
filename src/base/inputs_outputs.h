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
#include <vector>

#include "base/stream_config.h"

#include "base/input_uri.h"   // for InputUri
#include "base/output_uri.h"  // for OutputUri

namespace fastocloud {

typedef std::vector<InputUri> input_t;
typedef std::vector<OutputUri> output_t;

bool read_input(const StreamConfig& config, input_t* input);
bool read_output(const StreamConfig& config, output_t* output);

}  // namespace fastocloud
