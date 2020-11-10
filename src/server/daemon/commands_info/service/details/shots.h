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

#include <common/error.h>

namespace fastocloud {
namespace server {
namespace service {

struct CpuShot {
  uint64_t cpu_usage;
  uint64_t cpu_limit;
};

long double GetCpuMachineLoad(const CpuShot& prev, const CpuShot& next);
CpuShot GetMachineCpuShot();

struct MemoryShot {
  MemoryShot();

  size_t ram_bytes_total;
  size_t ram_bytes_free;
};

MemoryShot GetMachineMemoryShot();

struct HddShot {
  HddShot();

  size_t hdd_bytes_total;
  size_t hdd_bytes_free;
};

HddShot GetMachineHddShot();

struct NetShot {
  NetShot();

  size_t bytes_recv;
  size_t bytes_send;
};

NetShot GetMachineNetShot();

struct SysinfoShot {
  SysinfoShot();

  unsigned long loads[3];
  time_t uptime;
};

SysinfoShot GetMachineSysinfoShot();

}  // namespace service
}  // namespace server
}  // namespace fastocloud
