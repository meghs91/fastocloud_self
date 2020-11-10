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

#include <condition_variable>

#include "server/gpu_stats/perf_monitor.h"

namespace fastocloud {
namespace server {
namespace gpu_stats {

class IntelMonitor : public IPerfMonitor {
 public:
  explicit IntelMonitor(int* load);
  ~IntelMonitor() override;

  bool Exec() override;
  void Stop() override;

  static bool IsGpuAvailable();

 private:
  int* load_;
  std::mutex stop_mutex_;
  std::condition_variable stop_cond_;
  bool stop_flag_;
};

}  // namespace gpu_stats
}  // namespace server
}  // namespace fastocloud
