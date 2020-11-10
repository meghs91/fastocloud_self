/*  Copyright (C) 2014-2017 FastoGT. All right reserved.
    This file is part of iptv_cloud.
    iptv_cloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    iptv_cloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with iptv_cloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "server/gpu_stats/nvidia_monitor.h"

#include <nvml.h>

namespace fastocloud {
namespace server {
namespace gpu_stats {

NvidiaMonitor::NvidiaMonitor(int* load) : load_(load), stop_mutex_(), stop_cond_(), stop_flag_(false) {}

NvidiaMonitor::~NvidiaMonitor() {}

bool NvidiaMonitor::IsGpuAvailable() {
  nvmlReturn_t ret = nvmlInit();
  if (ret != NVML_SUCCESS) {
    return false;
  }

  unsigned devices_num = 0;
  ret = nvmlDeviceGetCount(&devices_num);
  if (ret != NVML_SUCCESS || devices_num == 0) {
    nvmlShutdown();
    return false;
  }

  nvmlShutdown();
  return true;
}

bool NvidiaMonitor::Exec() {
  if (!load_) {
    nvmlShutdown();
    return false;
  }

  nvmlReturn_t ret = nvmlInit();
  if (ret != NVML_SUCCESS) {
    return false;
  }

  unsigned devices_num = 0;
  ret = nvmlDeviceGetCount(&devices_num);
  if (ret != NVML_SUCCESS || devices_num == 0) {
    nvmlShutdown();
    return false;
  }

  std::unique_lock<std::mutex> lock(stop_mutex_);
  while (!stop_flag_) {
    uint64_t total_enc = 0, total_dec = 0;
    for (unsigned i = 0; i != devices_num; ++i) {
      unsigned enc, dec, enc_sampling, dec_sampling;
      nvmlDevice_t device;
      ret = nvmlDeviceGetHandleByIndex(i, &device);
      if (ret != NVML_SUCCESS) {
        continue;
      }
      ret = nvmlDeviceGetDecoderUtilization(device, &dec, &dec_sampling);
      if (ret != NVML_SUCCESS) {
        continue;
      }
      ret = nvmlDeviceGetEncoderUtilization(device, &enc, &enc_sampling);
      if (ret != NVML_SUCCESS) {
        continue;
      }
      total_dec += dec;
      total_enc += enc;
    }
    *load_ = (total_dec + total_enc) / (devices_num * 2);

    std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(1));
    if (interrupt_status == std::cv_status::no_timeout) {  // if notify
      if (stop_flag_) {
        break;
      }
    }
  }

  nvmlShutdown();
  return true;
}

void NvidiaMonitor::Stop() {
  std::unique_lock<std::mutex> lock(stop_mutex_);
  stop_flag_ = true;
  stop_cond_.notify_one();
}

}  // namespace gpu_stats
}  // namespace server
}  // namespace fastocloud
