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
#include <thread>
#include <vector>

#include <common/libev/io_loop.h>
#include <common/libev/io_loop_observer.h>
#include <common/process/process_metrics.h>
#include <common/threads/barrier.h>

#include <fastotv/protocol/protocol.h>
#include <fastotv/protocol/types.h>

#include "base/stream_config.h"
#include "stream/gstreamer_init.h"
#include "stream/ibase_stream.h"
#include "stream/timeshift.h"

namespace fastocloud {
namespace stream {

class StreamController : public common::libev::IoLoopObserver, public IBaseStream::IStreamClient {
 public:
  enum constants : uint32_t { restart_after_frozen_sec = 60 };

  StreamController(const common::file_system::ascii_directory_string_path& feedback_dir,
                   const common::file_system::ascii_file_string_path& streamlink_path,
                   fastotv::protocol::protocol_client_t* command_client,
                   StreamStruct* mem);

  common::Error Init(const StreamConfig& config_args);

  ~StreamController() override;

  int Exec();

 protected:
  virtual common::ErrnoError HandleRequestCommand(common::libev::IoClient* client,
                                                  const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  virtual common::ErrnoError HandleResponceCommand(common::libev::IoClient* client,
                                                   const fastotv::protocol::response_t* resp) WARN_UNUSED_RESULT;

 private:
  common::ErrnoError HandleRequestStopStream(common::libev::IoClient* client,
                                             const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestRestartStream(common::libev::IoClient* client,
                                                const fastotv::protocol::request_t* req) WARN_UNUSED_RESULT;

  void Stop();
  void Restart();

  void PreLooped(common::libev::IoLoop* loop) override;
  void PostLooped(common::libev::IoLoop* loop) override;
  void Accepted(common::libev::IoClient* client) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoClient* client) override;
  void Closed(common::libev::IoClient* client) override;

  void TimerEmited(common::libev::IoLoop* loop, common::libev::timer_id_t id) override;

  void Accepted(common::libev::IoChild* child) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  void ChildStatusChanged(common::libev::IoChild* child, int status, int signal) override;

  void DataReceived(common::libev::IoClient* client) override;
  void DataReadyToWrite(common::libev::IoClient* client) override;

  common::ErrnoError StreamDataRecived(common::libev::IoClient* client) WARN_UNUSED_RESULT;
  void StopStream();
  void RestartStream();

  void OnStatusChanged(IBaseStream* stream, StreamStatus status) override;
  GstPadProbeInfo* OnCheckReveivedData(IBaseStream* stream, InputProbe* probe, GstPadProbeInfo* info) override;
  GstPadProbeInfo* OnCheckReveivedOutputData(IBaseStream* stream, OutputProbe* probe, GstPadProbeInfo* info) override;
  void OnInputProbeEvent(IBaseStream* stream, InputProbe* probe, GstEvent* event) override;
  void OnOutputProbeEvent(IBaseStream* stream, OutputProbe* probe, GstEvent* event) override;
  void OnPipelineEOS(IBaseStream* stream) override;
  void OnTimeoutUpdated(IBaseStream* stream) override;
  void OnSyncMessageReceived(IBaseStream* stream, GstMessage* message) override;
  void OnASyncMessageReceived(IBaseStream* stream, GstMessage* message) override;
  void OnInputChanged(IBaseStream* stream, const InputUri& uri) override;
#if defined(MACHINE_LEARNING)
  void OnMlNotification(IBaseStream* stream, const std::vector<fastotv::commands_info::ml::ImageBox>& images) override;
#endif

  void OnPipelineCreated(IBaseStream* stream) override;

  common::ErrnoError SendResponceToParent(const std::string& cmd) WARN_UNUSED_RESULT;

  void DumpStreamStatus(StreamStruct* stat);

  const common::file_system::ascii_directory_string_path feedback_dir_;
  const common::file_system::ascii_file_string_path streamlink_path_;
  const Config* config_;
  TimeShiftInfo timeshift_info_;
  size_t restart_attempts_;

  std::mutex stop_mutex_;
  std::condition_variable stop_cond_;
  bool stop_;

  std::thread ev_thread_;
  common::libev::IoLoop* loop_;
  common::libev::timer_id_t ttl_master_timer_;
  common::threads::barrier libev_started_;

  StreamStruct* mem_;

  //
  IBaseStream* origin_;
  std::unique_ptr<common::process::ProcessMetrics> process_metrics_;

  GstInitializer init_;
};

}  // namespace stream
}  // namespace fastocloud
