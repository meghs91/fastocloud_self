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

#include "stream/stream_controller.h"

#if defined(OS_WIN)
#include <processthreadsapi.h>
#endif

#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>
#include <common/system_info/system_info.h>
#include <common/time.h>

#include "base/config_fields.h"  // for ID_FIELD
#include "base/constants.h"
#include "base/gst_constants.h"

#include "stream/configs_factory.h"
#include "stream/ibase_stream.h"
#include "stream/link_generator/streamlink.h"
#include "stream/probes.h"
#include "stream/stream_server.h"
#include "stream/streams/configs/relay_config.h"
#include "stream/streams_factory.h"  // for isTimeshiftP...

#include "stream_commands/commands.h"
#include "stream_commands/commands_factory.h"

namespace fastocloud {
namespace stream {

namespace {

TimeShiftInfo make_timeshift_info(const StreamConfig& config) {
  TimeShiftInfo tinfo;

  std::string timeshift_dir;
  common::Value* timeshift_dir_field = config->Find(TIMESHIFT_DIR_FIELD);
  if (!timeshift_dir_field || !timeshift_dir_field->GetAsBasicString(&timeshift_dir)) {
    CRITICAL_LOG() << "Define " TIMESHIFT_DIR_FIELD " variable and make it valid";
  }
  tinfo.timshift_dir = common::file_system::ascii_directory_string_path(timeshift_dir);

  int timeshift_chunk_life_time = 0;
  common::Value* timeshift_chunk_life_time_field = config->Find(TIMESHIFT_DIR_FIELD);
  if (timeshift_chunk_life_time_field && timeshift_chunk_life_time_field->GetAsInteger(&timeshift_chunk_life_time)) {
    tinfo.timeshift_chunk_life_time = timeshift_chunk_life_time;
  }

  int timeshift_delay = 0;
  common::Value* timeshift_delay_field = config->Find(TIMESHIFT_DELAY_FIELD);
  if (timeshift_delay_field && timeshift_delay_field->GetAsInteger(&timeshift_delay)) {
    tinfo.timeshift_delay = timeshift_delay;
  }
  return tinfo;
}

}  // namespace

StreamController::StreamController(const common::file_system::ascii_directory_string_path& feedback_dir,
                                   const common::file_system::ascii_file_string_path& streamlink_path,
                                   fastotv::protocol::protocol_client_t* command_client,
                                   StreamStruct* mem)
    : IBaseStream::IStreamClient(),
      feedback_dir_(feedback_dir),
      streamlink_path_(streamlink_path),
      config_(nullptr),
      timeshift_info_(),
      restart_attempts_(0),
      stop_mutex_(),
      stop_cond_(),
      stop_(false),
      ev_thread_(),
      loop_(new StreamServer(command_client, this)),
      ttl_master_timer_(0),
      libev_started_(2),
      mem_(mem),
      origin_(nullptr),
#if defined(OS_WIN)
      process_metrics_(common::process::ProcessMetrics::CreateProcessMetrics(GetCurrentProcess()))
#else
      process_metrics_(common::process::ProcessMetrics::CreateProcessMetrics(getpid()))
#endif
{
  CHECK(mem);
  loop_->SetName("main");
}

common::Error StreamController::Init(const StreamConfig& config_args) {
  Config* lconfig = nullptr;
  common::Error err = make_config(config_args, &lconfig);
  if (err) {
    return err;
  }

  config_ = lconfig;
  fastotv::StreamType stream_type = config_->GetType();
  if (stream_type == fastotv::TIMESHIFT_RECORDER || stream_type == fastotv::TIMESHIFT_PLAYER ||
      stream_type == fastotv::CATCHUP) {
    timeshift_info_ = make_timeshift_info(config_args);
  }

  EncoderType enc = CPU;
  std::string video_codec;
  common::Value* vcodec = config_args->Find(VIDEO_CODEC_FIELD);
  if (vcodec && vcodec->GetAsBasicString(&video_codec)) {
    EncoderType lenc;
    if (GetEncoderType(video_codec, &lenc)) {
      enc = lenc;
    }
  }

  init_.Init(0, nullptr, enc);
  if (enc == GPU_NVIDIA) {
    /*if (!init_.SetPluginAsPrimary("nvdec", 10)) {
      WARNING_LOG() << "Failed to update nvdec priority";
    }*/
  }
  return common::Error();
}

StreamController::~StreamController() {
  loop_->Stop();
  ev_thread_.join();

  destroy(&loop_);
  init_.Deinit();
  destroy(&config_);
}

int StreamController::Exec() {
  const link_generator::StreamLinkGenerator gena(streamlink_path_);
  ev_thread_ = std::thread([this] {
    int res = loop_->Exec();
    UNUSED(res);
  });
  libev_started_.Wait();

  while (!stop_) {
    chunk_index_t start_chunk_index = invalid_chunk_index;
    if (config_->GetType() == fastotv::TIMESHIFT_PLAYER) {  // if timeshift player or cathcup player
      const streams::TimeshiftConfig* tconfig = static_cast<const streams::TimeshiftConfig*>(config_);
      time_t timeshift_chunk_duration = tconfig->GetTimeShiftChunkDuration();
      while (!timeshift_info_.FindChunkToPlay(timeshift_chunk_duration, &start_chunk_index)) {
        mem_->status = WAITING;
        DumpStreamStatus(mem_);

        {
          std::unique_lock<std::mutex> lock(stop_mutex_);
          std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(timeshift_chunk_duration));
          if (interrupt_status == std::cv_status::no_timeout) {  // if notify
            mem_->restarts++;
            break;
          }
        }
      }

      INFO_LOG() << "Founded chunk index " << start_chunk_index;
      if (start_chunk_index == invalid_chunk_index) {
        continue;
      }
    }

    int stabled_status = EXIT_SUCCESS;
    const int signal_number = 0;
    const fastotv::timestamp_t start_utc_now = common::time::current_utc_mstime();
    const std::unique_ptr<Config> config_copy(make_config_copy(config_, &gena));
    origin_ =
        StreamsFactory::GetInstance().CreateStream(config_copy.get(), this, mem_, timeshift_info_, start_chunk_index);
    if (!origin_) {
      CRITICAL_LOG() << "Can't create stream";
      break;
    }

    const bool is_vod = origin_->IsVod();
    const ExitStatus res = origin_->Exec();
    destroy(&origin_);
    if (res == EXIT_INNER) {
      stabled_status = EXIT_FAILURE;
    }

    const fastotv::timestamp_t end_utc_now = common::time::current_utc_mstime();
    const fastotv::timestamp_t diff_utc_time = end_utc_now - start_utc_now;
    INFO_LOG() << "Stream exit with status: " << (stabled_status ? "FAILURE" : "SUCCESS")
               << ", signal: " << signal_number << ", working time: " << diff_utc_time << " msec.";
    if (is_vod) {
      break;
    }

    bool is_longer_work = diff_utc_time > restart_after_frozen_sec * 10 * 1000;
    if (stabled_status != EXIT_SUCCESS && !is_longer_work) {
      mem_->idle_time += diff_utc_time;
    }

    if (stabled_status == EXIT_SUCCESS) {
      restart_attempts_ = 0;
      continue;
    }

    if (is_longer_work) {  // if longer work
      restart_attempts_ = 0;
      continue;
    }

    size_t wait_time = 0;
    if (++restart_attempts_ == config_->GetMaxRestartAttempts()) {
      restart_attempts_ = 0;
      mem_->status = FROZEN;
      DumpStreamStatus(mem_);
      wait_time = restart_after_frozen_sec;
    } else {
      wait_time = restart_attempts_ * (restart_after_frozen_sec / config_->GetMaxRestartAttempts());
    }

    INFO_LOG() << "Automatically restarted after " << wait_time << " seconds, stream restarts: " << mem_->restarts
               << ", attempts: " << restart_attempts_;

    std::unique_lock<std::mutex> lock(stop_mutex_);
    std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(wait_time));
    if (interrupt_status == std::cv_status::no_timeout) {  // if notify
      restart_attempts_ = 0;
    } else {
      mem_->idle_time += wait_time * 1000;
    }
  }

  return EXIT_SUCCESS;
}

void StreamController::Stop() {
  {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    stop_ = true;
    stop_cond_.notify_all();
  }
  StopStream();
}

void StreamController::Restart() {
  {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    stop_cond_.notify_all();
  }
  StopStream();
}

void StreamController::PreLooped(common::libev::IoLoop* loop) {
  UNUSED(loop);
  const auto ttl_sec = config_->GetTimeToLifeStream();
  if (ttl_sec && *ttl_sec) {
    ttl_master_timer_ = loop_->CreateTimer(*ttl_sec, false);
    NOTICE_LOG() << "Set stream ttl: " << *ttl_sec;
  }

  libev_started_.Wait();
  INFO_LOG() << "Child listening started!";
}

void StreamController::PostLooped(common::libev::IoLoop* loop) {
  UNUSED(loop);
  if (ttl_master_timer_) {
    loop_->RemoveTimer(ttl_master_timer_);
  }
  INFO_LOG() << "Child listening finished!";
}

void StreamController::Accepted(common::libev::IoClient* client) {
  UNUSED(client);
}

void StreamController::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
}

void StreamController::Closed(common::libev::IoClient* client) {
  UNUSED(client);
  Stop();
}

common::ErrnoError StreamController::StreamDataRecived(common::libev::IoClient* client) {
  std::string input_command;
  fastotv::protocol::protocol_client_t* pclient = static_cast<fastotv::protocol::protocol_client_t*>(client);
  common::ErrnoError err = pclient->ReadCommand(&input_command);
  if (err) {  // i don't want handle spam, command must be formated according
              // protocol
    return err;
  }

  fastotv::protocol::request_t* req = nullptr;
  fastotv::protocol::response_t* resp = nullptr;
  common::Error err_parse = common::protocols::json_rpc::ParseJsonRPC(input_command, &req, &resp);
  if (err_parse) {
    const std::string err_str = err_parse->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  if (req) {
    INFO_LOG() << "Received request: " << input_command;
    err = HandleRequestCommand(pclient, req);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete req;
  } else if (resp) {
    INFO_LOG() << "Received responce: " << input_command;
    err = HandleResponceCommand(pclient, resp);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete resp;
  } else {
    NOTREACHED();
    return common::make_errno_error("Invalid command type.", EINVAL);
  }
  return common::ErrnoError();
}

void StreamController::DataReceived(common::libev::IoClient* client) {
  auto err = StreamDataRecived(client);
  if (err) {
    DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    // client->Close();
    // delete client;
    Stop();
  }
}

void StreamController::DataReadyToWrite(common::libev::IoClient* client) {
  UNUSED(client);
}

void StreamController::TimerEmited(common::libev::IoLoop* loop, common::libev::timer_id_t id) {
  UNUSED(loop);
  if (id == ttl_master_timer_) {
    const auto ttl_sec = config_->GetTimeToLifeStream();
    if (ttl_sec) {
      NOTICE_LOG() << "Timeout notified ttl was: " << *ttl_sec;
    }
    Stop();
  }
}

void StreamController::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void StreamController::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void StreamController::ChildStatusChanged(common::libev::IoChild* child, int status, int signal) {
  UNUSED(child);
  UNUSED(status);
  UNUSED(signal);
}

common::ErrnoError StreamController::HandleRequestCommand(common::libev::IoClient* client,
                                                          const fastotv::protocol::request_t* req) {
  if (req->method == STOP_STREAM) {
    return HandleRequestStopStream(client, req);
  } else if (req->method == RESTART_STREAM) {
    return HandleRequestRestartStream(client, req);
  }

  WARNING_LOG() << "Received unknown command: " << req->method;
  return common::ErrnoError();
}

common::ErrnoError StreamController::HandleResponceCommand(common::libev::IoClient* client,
                                                           const fastotv::protocol::response_t* resp) {
  CHECK(loop_->IsLoopThread());

  fastotv::protocol::protocol_client_t* pclient = static_cast<fastotv::protocol::protocol_client_t*>(client);
  fastotv::protocol::request_t req;
  if (pclient->PopRequestByID(resp->id, &req)) {
    if (req.method == STATISTIC_STREAM) {
    } else if (req.method == CHANGED_SOURCES_STREAM) {
#if defined(MACHINE_LEARNING)
    } else if (req.method == ML_NOTIFICATION_STREAM) {
#endif
    } else {
      WARNING_LOG() << "HandleResponceStreamsCommand not handled command: " << req.method;
    }
  }
  return common::ErrnoError();
}

common::ErrnoError StreamController::HandleRequestStopStream(common::libev::IoClient* client,
                                                             const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  fastotv::protocol::protocol_client_t* pclient = static_cast<fastotv::protocol::protocol_client_t*>(client);
  fastotv::protocol::response_t resp = StopStreamResponseSuccess(req->id);
  ignore_result(pclient->WriteResponse(resp));
  Stop();
  return common::ErrnoError();
}

common::ErrnoError StreamController::HandleRequestRestartStream(common::libev::IoClient* client,
                                                                const fastotv::protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  fastotv::protocol::protocol_client_t* pclient = static_cast<fastotv::protocol::protocol_client_t*>(client);
  fastotv::protocol::response_t resp = RestartStreamResponseSuccess(req->id);
  ignore_result(pclient->WriteResponse(resp));
  Restart();
  return common::ErrnoError();
}

void StreamController::StopStream() {
  if (origin_) {
    origin_->Quit(EXIT_SELF);
  }
}

void StreamController::RestartStream() {
  if (origin_) {
    origin_->Restart();
  }
}

void StreamController::OnStatusChanged(IBaseStream* stream, StreamStatus status) {
  UNUSED(status);
  DumpStreamStatus(stream->GetStats());
}

GstPadProbeInfo* StreamController::OnCheckReveivedData(IBaseStream* stream, InputProbe* probe, GstPadProbeInfo* info) {
  UNUSED(stream);
  UNUSED(probe);
  return info;
}

GstPadProbeInfo* StreamController::OnCheckReveivedOutputData(IBaseStream* stream,
                                                             OutputProbe* probe,
                                                             GstPadProbeInfo* info) {
  UNUSED(stream);
  UNUSED(probe);
  return info;
}

void StreamController::OnInputProbeEvent(IBaseStream* stream, InputProbe* probe, GstEvent* event) {
  UNUSED(stream);
  UNUSED(probe);
  UNUSED(event);
}

void StreamController::OnOutputProbeEvent(IBaseStream* stream, OutputProbe* probe, GstEvent* event) {
  UNUSED(stream);
  UNUSED(probe);
  UNUSED(event);
}

void StreamController::OnPipelineEOS(IBaseStream* stream) {
  if (stream->IsVod()) {
    stream->Quit(EXIT_SELF);
  } else {
    stream->Quit(EXIT_INNER);
  }
}

void StreamController::OnTimeoutUpdated(IBaseStream* stream) {
  DumpStreamStatus(stream->GetStats());
}

void StreamController::OnASyncMessageReceived(IBaseStream* stream, GstMessage* message) {
  UNUSED(stream);
  UNUSED(message);
}

void StreamController::OnSyncMessageReceived(IBaseStream* stream, GstMessage* message) {
  UNUSED(stream);
  UNUSED(message);
}

void StreamController::OnInputChanged(IBaseStream* stream, const InputUri& uri) {
  UNUSED(stream);
  ChangedSouresInfo ch(mem_->id, uri);
  static_cast<StreamServer*>(loop_)->SendChangeSourcesBroadcast(ch);
}

#if defined(MACHINE_LEARNING)
void StreamController::OnMlNotification(IBaseStream* stream,
                                        const std::vector<fastotv::commands_info::ml::ImageBox>& images) {
  UNUSED(stream);
  fastotv::commands_info::ml::NotificationInfo notif(mem_->id, images);
  static_cast<StreamServer*>(loop_)->SendMlNotificationBroadcast(notif);
}
#endif

void StreamController::OnPipelineCreated(IBaseStream* stream) {
  auto dump_file = feedback_dir_.MakeFileStringPath(DUMP_FILE_NAME);
  if (dump_file) {
    const auto path = *dump_file;
    bool dumped = stream->DumpIntoFile(path);
    if (!dumped) {
      WARNING_LOG() << "Can't create pipeline graph, path: " << path.GetPath();
    }
  }
}

void StreamController::DumpStreamStatus(StreamStruct* stat) {
  const double cpu_load = process_metrics_->GetPlatformIndependentCPUUsage();
#if defined(OS_LINUX) || defined(OS_ANDROID)
  const size_t rss = process_metrics_->GetResidentSetSize();
#else
#pragma message "Please implement"
  const size_t rss = 0;
#endif
  const fastotv::timestamp_t current_time = common::time::current_utc_mstime();
  StatisticInfo statistic(*stat, cpu_load, rss, current_time);
  static_cast<StreamServer*>(loop_)->SendStatisticBroadcast(statistic);
}

}  // namespace stream
}  // namespace fastocloud
