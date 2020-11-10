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

#include "server/daemon/commands_info/service/details/shots.h"

#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>

#include <string>

#if defined(OS_LINUX)
#include <sys/sysinfo.h>
#endif

#if defined(OS_MACOSX)
#include <mach/mach.h>
#endif

#if defined(OS_WIN)
#include <windows.h>
#endif

#if defined(OS_MACOSX) || defined(OS_FREEBSD)
#include <sys/resource.h>
#include <sys/sysctl.h>
#endif

#include <common/system_info/system_info.h>
#include <common/time.h>

#define CPU_FORMAT "cpu %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64

#if defined(OS_WIN)
namespace {

uint64_t FiletimeToInt(const FILETIME& ft) {
  ULARGE_INTEGER i;
  i.LowPart = ft.dwLowDateTime;
  i.HighPart = ft.dwHighDateTime;
  return i.QuadPart;
}

}  // namespace
#endif

namespace fastocloud {
namespace server {
namespace service {

long double GetCpuMachineLoad(const CpuShot& prev, const CpuShot& next) {
  long double total = next.cpu_limit - prev.cpu_limit;
  if (total == 0.0l) {
    return 0;
  }

  long double busy = next.cpu_usage - prev.cpu_usage;
  return (busy / total) * 100;
}

CpuShot GetMachineCpuShot() {
  uint64_t busy = 0, total = 0;
#if defined(OS_LINUX)
  FILE* fp = fopen("/proc/stat", "r");
  uint64_t user, nice, system, idle;
  if (fscanf(fp, CPU_FORMAT, &user, &nice, &system, &idle) != 4) {
    // Something bad happened with the information, so assume it's all invalid
    user = nice = system = idle = 0;
  }
  fclose(fp);
  busy = user + nice + system;
  total = busy + idle;
#elif defined(OS_MACOSX)
  host_cpu_load_info_data_t cpuinfo;
  mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
  if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
    for (int i = 0; i < CPU_STATE_MAX; i++) {
      total += cpuinfo.cpu_ticks[i];
    }
    busy = total - cpuinfo.cpu_ticks[CPU_STATE_IDLE];
  }
#elif defined(OS_WINDOWS)
  FILETIME idle, kernel, user;
  if (GetSystemTimes(&idle, &kernel, &user) != 0) {
    total = FiletimeToInt(kernel) + FiletimeToInt(user);
    busy = total - FiletimeToInt(idle);
  }
#else
#pragma message "Please implement"
#endif
  return {busy, total};
}

MemoryShot::MemoryShot() : ram_bytes_total(0), ram_bytes_free(0) {}

MemoryShot GetMachineMemoryShot() {
  MemoryShot shot;
  const auto total = common::system_info::AmountOfPhysicalMemory();
  if (total) {
    shot.ram_bytes_total = *total;
  }
  const auto avail = common::system_info::AmountOfAvailablePhysicalMemory();
  if (avail) {
    shot.ram_bytes_free = *avail;
  }
  return shot;
}

HddShot::HddShot() : hdd_bytes_total(0), hdd_bytes_free(0) {}

HddShot GetMachineHddShot() {
  HddShot sh;
  const auto total = common::system_info::AmountOfTotalDiskSpace("/");
  if (total) {
    sh.hdd_bytes_total = *total;
  }
  const auto avail = common::system_info::AmountOfFreeDiskSpace("/");
  if (avail) {
    sh.hdd_bytes_free = *avail;
  }
  return sh;
}

NetShot::NetShot() : bytes_recv(0), bytes_send(0) {}

NetShot GetMachineNetShot() {
  NetShot shot;
#if defined(OS_LINUX)
  FILE* netinfo = fopen("/proc/net/dev", "r");
  if (!netinfo) {
    return shot;
  }

  char line[512];
  char interf[128] = {0};
  int pos = 0;
  while (fgets(line, sizeof(line), netinfo)) {
    // face |bytes    packets errs drop fifo frame compressed multicast|
    // bytes    packets errs drop fifo colls carrier compressed
    if (pos > 1) {
      unsigned long long int r_bytes, r_packets, r_errs, r_drop, r_fifo, r_frame, r_compressed, r_multicast;
      unsigned long long int s_bytes, s_packets, s_errs, s_drop, s_fifo, s_colls, s_carrier, s_compressed;
      sscanf(line,
             "%s %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu "
             "%16llu %16llu %16llu "
             "%16llu %16llu %16llu %16llu %16llu",
             interf, &r_bytes, &r_packets, &r_errs, &r_drop, &r_fifo, &r_frame, &r_compressed, &r_multicast, &s_bytes,
             &s_packets, &s_errs, &s_drop, &s_fifo, &s_colls, &s_carrier, &s_compressed);
      if (strncmp(interf, "lo", 2) != 0) {
        shot.bytes_recv += r_bytes;
        shot.bytes_send += s_bytes;
      }
      memset(interf, 0, sizeof(interf));
    }
    pos++;
  }

  fclose(netinfo);
  return shot;
#else
#pragma message "Please implement"
  return shot;
#endif
}

SysinfoShot::SysinfoShot() : loads{0}, uptime(0) {}

SysinfoShot GetMachineSysinfoShot() {
  SysinfoShot inf;
#if defined(OS_LINUX)
  struct sysinfo info;
  int res = sysinfo(&info);
  if (res == ERROR_RESULT_VALUE) {
    return inf;
  }

  memcpy(&inf.loads, &info.loads, sizeof(unsigned long) * SIZEOFMASS(info.loads));
  inf.uptime = info.uptime;
  return inf;
#elif defined(OS_MACOSX) || defined(OS_FREEBSD)
  struct loadavg load;
  size_t load_len = sizeof(load);
  if (sysctlbyname("vm.loadavg", &load, &load_len, nullptr, 0) < 0) {
    return inf;
  }
  inf.loads[0] = load.ldavg[0] / load.fscale;
  inf.loads[1] = load.ldavg[1] / load.fscale;
  inf.loads[2] = load.ldavg[2] / load.fscale;

  struct timeval boottime;
  size_t boottime_len = sizeof(boottime);
  if (sysctlbyname("kern.boottime", &boottime, &boottime_len, nullptr, 0) < 0) {
    return inf;
  }
  inf.uptime = (common::time::current_utc_mstime() - common::time::timeval2mstime(&boottime)) / 1000;
  return inf;
#else
#pragma message "Please implement"
  return inf;
#endif
}

}  // namespace service
}  // namespace server
}  // namespace fastocloud
