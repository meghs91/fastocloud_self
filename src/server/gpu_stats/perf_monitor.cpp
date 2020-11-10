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

#include "server/gpu_stats/perf_monitor.h"

#include <common/macros.h>

#ifdef HAVE_NVML
#include "server/gpu_stats/nvidia_monitor.h"
#endif
#ifdef HAVE_CTT_METRICS
#include "server/gpu_stats/intel_monitor.h"
#endif

#define MAX_LEN 1024

namespace {

bool IsNvidiaGpuAvailable() {
#ifdef HAVE_NVML
  return fastocloud::server::gpu_stats::NvidiaMonitor::IsGpuAvailable();
#else
  return false;
#endif
}

bool IsIntelGpuAvailable() {
#ifdef HAVE_CTT_METRICS
  return fastocloud::server::gpu_stats::IntelMonitor::IsGpuAvailable();
#else
  return false;
#endif
}
}  // namespace

namespace fastocloud {
namespace server {
namespace gpu_stats {

IPerfMonitor::~IPerfMonitor() {}

IPerfMonitor* CreatePerfMonitor(int* load) {
#if !defined(HAVE_NVML) && !defined(HAVE_CTT_METRICS)
  UNUSED(load);
#endif
  if (IsNvidiaGpuAvailable()) {
#if defined(HAVE_NVML)
    return new NvidiaMonitor(load);
#else
    return nullptr;
#endif
  } else if (IsIntelGpuAvailable()) {
#if defined(HAVE_CTT_METRICS)
    return new IntelMonitor(load);
#else
    return nullptr;
#endif
  }
  return nullptr;
}

}  // namespace gpu_stats
}  // namespace server
}  // namespace fastocloud
