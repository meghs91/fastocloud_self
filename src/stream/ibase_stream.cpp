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

#include "stream/ibase_stream.h"

#include <gst/base/gstbasesrc.h>  // for GstBaseSrc
#include <gst/video/video.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <common/sprintf.h>
#include <common/time.h>

#include "base/channel_stats.h"
#include "base/utils.h"

#include "stream/dumpers/dumpers_factory.h"
#include "stream/elements/element.h"
#include "stream/elements/sink/http.h"
#include "stream/gstreamer_utils.h"
#include "stream/ibase_builder.h"
#include "stream/probes.h"  // for Probe (ptr only), PROBE_IN, PROBE_OUT

#define MIN_OUT_DATA(SEC) 4 * 1024 * SEC  // 4 kBps
#define MIN_IN_DATA(SEC) 4 * 1024 * SEC   // 4 kBps
#define DEFAULT_FRAMERATE 25

#if defined(OS_WIN)
int setenv(const char* key, const char* value, int set) {
  UNUSED(set);
  std::string line = common::MemSPrintf("%s=%s", key, value);
  return putenv(line.c_str());
}
#endif

namespace {

void remove_notify_callback(gpointer user_data) {
  UNUSED(user_data);
}

struct ctx_holder {
  static ctx_holder* instance() {
    static ctx_holder inst;
    return &inst;
  }
  GMainContext* ctx;

 private:
  ctx_holder() {
    ctx = g_main_context_default();  // 1 ref
  }
  ~ctx_holder() {
    g_main_context_unref(ctx);  //
  }
};

}  // namespace

namespace fastocloud {
namespace stream {

IBaseStream::IStreamClient::~IStreamClient() {}

IBaseStream::IBaseStream(const Config* config, IStreamClient* client, StreamStruct* stats)
    : common::IMetaClassInfo(),
      client_(client),
      config_(config),
      probe_in_(),
      probe_out_(),
      loop_(g_main_loop_new(ctx_holder::instance()->ctx, FALSE)),
      pipeline_(nullptr),
      status_tick_(0),
      no_data_panic_tick_(0),
      stats_(stats),
      last_exit_status_(EXIT_INNER),
      is_live_(false),
      flags_(INITED_NOTHING),
      desire_flags_(INITED_NOTHING) {
  /*INFO_LOG() << "Api inited input: " <<
     common::ConvertToString(api_->GetUrl())
             << ", output: " << common::ConvertToString(api_->GetOutput());*/
  SetStatus(NEW);
  CHECK(config) << "Config must be!";
  CHECK(stats && stats->IsValid()) << "Stats must be!";
  CHECK_EQ(GetType(), config_->GetType());
}

void IBaseStream::LinkInputPad(GstPad* pad, element_id_t id, const common::uri::GURL& url) {
  DEBUG_LOG() << "InputPad created id: " << id << ", url: " << url.spec();
  InputProbe* probe = new InputProbe(id, url, this);
  probe->Link(pad);
  probe_in_.push_back(probe);
}

void IBaseStream::LinkOutputPad(GstPad* pad, element_id_t id, const common::uri::GURL& url, bool need_push) {
  DEBUG_LOG() << "OutputPad created id: " << id << ", url: " << url.spec();
  OutputProbe* probe = new OutputProbe(id, url, need_push, this);
  probe->Link(pad);
  probe_out_.push_back(probe);
}

void IBaseStream::PreExecCleanup(time_t old_life_time) {
  const fastotv::timestamp_t cur_timestamp = common::time::current_utc_mstime();
  const fastotv::timestamp_t max_life_time = IsVod() ? cur_timestamp : cur_timestamp - old_life_time * 1000;
  for (const OutputUri& output : config_->GetOutput()) {
    auto uri = output.GetUrl();
    if (uri.SchemeIsHTTPOrHTTPS()) {
      const auto http_path = output.GetHttpRoot();
      if (http_path) {
        RemoveOldFilesByTime(*http_path, max_life_time / 1000, "*" CHUNK_EXT);
      }
    }
  }
}

void IBaseStream::PostExecCleanup() {}

bool IBaseStream::InitPipeLine() {
  IBaseBuilder* builder = CreateBuilder();
  if (!builder->CreatePipeLine(&pipeline_, &pipeline_elements_)) {
    delete builder;
    return false;
  }

  if (client_) {
    client_->OnPipelineCreated(this);
  }

  delete builder;
  DEBUG_LOG() << "Pipeline for: " << ClassName() << " created";
  return true;
}

void IBaseStream::ClearOutProbes() {
  for (OutputProbe* probe : probe_out_) {
    delete probe;
  }
  probe_out_.clear();
}

void IBaseStream::ClearInProbes() {
  for (InputProbe* probe : probe_in_) {
    delete probe;
  }
  probe_in_.clear();
}

size_t IBaseStream::CountInputEOS() const {
  size_t count_in_eos = 0;
  std::map<element_id_t, Consistency> probes_statuses;
  for (Probe* probe : probe_in_) {
    element_id_t id = probe->GetID();
    Consistency state = probe->GetConsistency();
    auto it = probes_statuses.find(id);
    if (it != probes_statuses.end()) {
      it->second.eos &= state.eos;
    } else {
      probes_statuses.insert(std::make_pair(id, state));
    }
  }

  for (auto probe_state : probes_statuses) {
    if (probe_state.second.eos) {
      count_in_eos++;
    }
  }

  return count_in_eos;
}

size_t IBaseStream::CountOutEOS() const {
  size_t count_out_eos = 0;
  std::map<element_id_t, Consistency> probes_statuses;
  for (Probe* probe : probe_out_) {
    element_id_t id = probe->GetID();
    Consistency state = probe->GetConsistency();
    auto it = probes_statuses.find(id);
    if (it != probes_statuses.end()) {
      it->second.eos &= state.eos;
    } else {
      probes_statuses.insert(std::make_pair(id, state));
    }
  }

  for (auto probe_state : probes_statuses) {
    if (probe_state.second.eos) {
      count_out_eos++;
    }
  }

  return count_out_eos;
}

const char* IBaseStream::ClassName() const {
  return "IBaseStream";
}

IBaseStream::~IBaseStream() {
  g_main_loop_unref(loop_);
  ClearOutProbes();
  ClearInProbes();
  for (elements::Element* el : pipeline_elements_) {
    delete el;
  }
  pipeline_elements_.clear();
  // pipeline

  SetPipelineState(GST_STATE_NULL);
  if (pipeline_) {
    g_object_unref(pipeline_);
    pipeline_ = nullptr;
  }
  SetStatus(NEW);
}

ExitStatus IBaseStream::Exec() {
  PreExecCleanup(cleanup_life_period_sec);

  if (!InitPipeLine()) {
    return EXIT_INNER;
  }

  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
  guint main_timeout_id = g_timeout_add(main_timer_msecs, main_timer_callback, this);

  gst_bus_set_sync_handler(bus, sync_bus_callback, this, remove_notify_callback);
  guint bus_watch_id = gst_bus_add_watch(bus, async_bus_callback, this);
  gst_object_unref(bus);
  SetStatus(INIT);

  stats_->loop_start_time = common::time::current_utc_mstime();
  ResetDataWait();

  Play();

  // stream run
  PreLoop();
  g_main_loop_run(loop_);
  PostLoop(last_exit_status_);

  bool res = g_source_remove(bus_watch_id);
  DCHECK(res);
  res = g_source_remove(main_timeout_id);
  DCHECK(res);

  SetStatus(INIT);  // emulating loop statuses
  Stop();

  stats_->restarts++;
  PostExecCleanup();
  return last_exit_status_;
}

bool IBaseStream::IsLive() const {
  return is_live_;
}

bool IBaseStream::IsAudioInited() const {
  return flags_ & INITED_AUDIO;
}

bool IBaseStream::IsVideoInited() const {
  return flags_ & INITED_VIDEO;
}

void IBaseStream::RegisterAudioCaps(SupportedAudioCodec saudio, GstCaps* caps, element_id_t id) {
  UNUSED(saudio);
  bool is_inited_audio = desire_flags_ & INITED_AUDIO;
  if (is_inited_audio) {
    return;
  }

  GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
  if (pad_struct) {
    gint rate = 0, channels = 0;
    if (gst_structure_get_int(pad_struct, "rate", &rate) && gst_structure_get_int(pad_struct, "channels", &channels)) {
      common::media::DesireBytesPerSec kbps = common::media::CalculateDesireAudioBandwidthBytesPerSec(rate, channels);
      common::media::DesireBytesPerSec prev = stats_->input[id].GetDesireBytesPerSecond();
      common::media::DesireBytesPerSec next = prev + kbps;
      stats_->input[id].SetDesireBytesPerSecond(next);
      desire_flags_ |= INITED_AUDIO;
    }
  }
}

void IBaseStream::RegisterVideoCaps(SupportedVideoCodec svideo, GstCaps* caps, element_id_t id) {
  bool is_inited_video = desire_flags_ & INITED_VIDEO;
  if (is_inited_video) {
    return;
  }

  if (svideo == VIDEO_MPEG_CODEC) {
    GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
    if (pad_struct) {
      gint width = 0;
      gint height = 0;
      if (gst_structure_get_int(pad_struct, "width", &width) && gst_structure_get_int(pad_struct, "height", &height)) {
        common::media::DesireBytesPerSec kbps = common::media::CalculateDesireMPEGBandwidthBytesPerSec(width, height);
        common::media::DesireBytesPerSec prev = stats_->input[id].GetDesireBytesPerSecond();
        stats_->input[id].SetDesireBytesPerSecond(prev + kbps);
        desire_flags_ |= INITED_VIDEO;
      }
    }
  } else if (svideo == VIDEO_H264_CODEC) {
    GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
    if (pad_struct) {
      gint width = 0;
      gint height = 0;
      gint profile = 0;
      if (gst_structure_get_int(pad_struct, "width", &width) && gst_structure_get_int(pad_struct, "height", &height) &&
          gst_structure_get_int(pad_struct, "profile", &profile)) {
        gint framerate = DEFAULT_FRAMERATE;
        gint value_numerator = 0;
        gint value_denominator = 0;

        if (gst_structure_get_fraction(pad_struct, "framerate", &value_numerator, &value_denominator)) {
          framerate = value_numerator / value_denominator;
        }

        common::media::DesireBytesPerSec kbps =
            common::media::CalculateDesireH264BandwidthBytesPerSec(width, height, framerate, profile);
        common::media::DesireBytesPerSec prev = stats_->input[id].GetDesireBytesPerSecond();
        stats_->input[id].SetDesireBytesPerSecond(prev + kbps);
        desire_flags_ |= INITED_VIDEO;
      }
    }
  }
}

void IBaseStream::SetAudioInited(bool val) {
  if (val) {
    flags_ |= INITED_AUDIO;
  } else {
    flags_ &= INITED_AUDIO;
  }
}

void IBaseStream::SetVideoInited(bool val) {
  if (val) {
    flags_ |= INITED_VIDEO;
  } else {
    flags_ &= ~INITED_VIDEO;
  }
}

void IBaseStream::Restart() {
  stats_->loop_start_time = common::time::current_utc_mstime();
  ResetDataWait();

  Pause();
  SetStatus(INIT);  // emulating loop statuses
  Play();

  stats_->restarts++;
}

void IBaseStream::Stop() {
  SetPipelineState(GST_STATE_NULL);
}

void IBaseStream::Pause() {
  SetPipelineState(GST_STATE_PAUSED);
}

void IBaseStream::Play() {
  GstStateChangeReturn ret = SetPipelineState(GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    WARNING_LOG() << "Unable to set the pipeline to the playing state.";
  } else if (ret == GST_STATE_CHANGE_NO_PREROLL) {
    is_live_ = true;
  }
}

void IBaseStream::ResetDataWait() {
  no_data_panic_tick_ = GetElipsedTime() + no_data_panic_sec;  // update no_data_panic timestamp
  stats_->ResetDataWait();
}

void IBaseStream::Quit(ExitStatus status) {
  GstElement* pipeline = pipeline_;
  GstStructure* result = gst_structure_new("exit_info", "status", G_TYPE_INT, status, nullptr);
  GstMessage* message = gst_message_new_application(GST_OBJECT(pipeline), result);
  bool res = gst_element_post_message(pipeline, message);
  if (!res) {
    WARNING_LOG() << "Failed to post quit message.";
  }
}

StreamStruct* IBaseStream::GetStats() const {
  return stats_;
}

time_t IBaseStream::GetElipsedTime() const {
  const fastotv::timestamp_t current_time = common::time::current_utc_mstime();
  return (current_time - stats_->start_time) / 1000;
}

fastotv::StreamType IBaseStream::GetType() const {
  return stats_->type;
}

GstBusSyncReply IBaseStream::HandleSyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  UNUSED(bus);
  GstMessageType type = GST_MESSAGE_TYPE(message);
  GstObject* src = GST_MESSAGE_SRC(message);
  if (type == GST_MESSAGE_APPLICATION) {
    GstObject* pipeline = GST_OBJECT(pipeline_);
    if (src == pipeline && g_main_loop_is_running(loop_)) {
      const GstStructure* exit_status_struct = gst_message_get_structure(message);
      const GValue* status_val = gst_structure_get_value(exit_status_struct, "status");
      gint exit_status = gvalue_cast<gint>(status_val);
      WARNING_LOG() << "Received exit command, status: " << exit_status;
      // DCHECK(g_main_loop_is_running(loop_));
      g_main_loop_quit(loop_);
      last_exit_status_ = static_cast<ExitStatus>(exit_status);
    }
  } else if (type == GST_MESSAGE_ELEMENT) {
    const GstStructure* structure = gst_message_get_structure(message);
    const char* element_name = gst_structure_get_name(structure);
    if (strcmp(element_name, "GstMultiFileSink") == 0) {  // splitmuxsink-fragment-closed
      // GstClockTime running_time;
      const char* file_path_str = gst_structure_get_string(structure, "filename");
      UNUSED(file_path_str);
      // gst_structure_get_clock_time(structure, "running-time", &running_time);
      for (const OutputUri& output : config_->GetOutput()) {
        auto uri = output.GetUrl();
        if (uri.SchemeIsHTTPOrHTTPS()) {
          const auto hlst = output.GetHlsType();
          if (hlst && *hlst == OutputUri::HLS_PUSH) {
            // HLS PUSH
          }
        }
      }
    }
  }

  if (client_) {
    client_->OnSyncMessageReceived(this, message);
  }

  return GST_BUS_PASS;
}

void IBaseStream::OnOutputDataFailed() {
  bool is_lazy_streams = false;
  // #FIXME, sinks on pads not generate speed
  for (const OutputUri& output : config_->GetOutput()) {
    common::uri::GURL uri = output.GetUrl();
    if (uri.SchemeIsSrt() || uri.SchemeIsUdp()) {
      is_lazy_streams = true;
      break;
    }
  }

  if (!is_lazy_streams) {
    WARNING_LOG() << "There is no output data for a last " << no_data_panic_sec << " seconds.";
    Quit(EXIT_INNER);
  }
}

void IBaseStream::OnOutputDataOK() {
  ResetDataWait();
}

void IBaseStream::OnInputDataFailed() {
  WARNING_LOG() << "There is no input data for a last " << no_data_panic_sec << " seconds.";
  // handle streamlink
  Quit(EXIT_INNER);
}

void IBaseStream::OnInputDataOK() {}

gboolean IBaseStream::HandleMainTimerTick() {
  const time_t up_time = GetElipsedTime();
  const size_t diff = (no_data_panic_sec - no_data_panic_tick_ + up_time) + 1;

  size_t checkpoint_diff_in_total = 0;
  common::media::DesireBytesPerSec checkpoint_desire_in_total;
  size_t input_stream_count = stats_->input.size();
  for (size_t i = 0; i < input_stream_count; ++i) {
    size_t checkpoint_diff_out_stream = stats_->input[i].GetDiffTotalBytes();
    stats_->input[i].UpdateBps(diff);
    checkpoint_diff_in_total += checkpoint_diff_out_stream;
    checkpoint_desire_in_total += stats_->input[i].GetDesireBytesPerSecond();
  }

  size_t checkpoint_diff_out_total = 0;
  size_t output_stream_count = stats_->output.size();
  for (size_t i = 0; i < output_stream_count; ++i) {
    size_t checkpoint_diff_out_stream = stats_->output[i].GetDiffTotalBytes();
    stats_->output[i].UpdateBps(diff);
    checkpoint_diff_out_total += checkpoint_diff_out_stream;
  }

  if (up_time > no_data_panic_tick_) {  // check is stream in noraml state
    size_t count_in_eos = CountInputEOS();
    size_t count_out_eos = CountOutEOS();
    DEBUG_LOG() << "NoData checkpoint: input eos (" << count_in_eos << "/" << input_stream_count << "), output eos ("
                << count_out_eos << "/" << output_stream_count << "), received bytes " << checkpoint_diff_in_total
                << ", sended bytes " << checkpoint_diff_out_total;
    bool is_input_failed = checkpoint_diff_in_total < MIN_IN_DATA(no_data_panic_sec);
    if (is_input_failed) {
      OnInputDataFailed();
    } else {
      OnInputDataOK();
    }

    if (checkpoint_desire_in_total.IsValid() && desire_flags_ != INITED_NOTHING) {
      size_t in_bytes_per_sec = (checkpoint_diff_in_total / diff);
      if (!checkpoint_desire_in_total.InRange(in_bytes_per_sec)) {
        NOTICE_LOG() << "Input bandwidth not in range of desire speed " << checkpoint_desire_in_total.min
                     << " <= " << in_bytes_per_sec << " <= " << checkpoint_desire_in_total.max;
      }
    }

    bool is_output_failed = checkpoint_diff_out_total < MIN_OUT_DATA(no_data_panic_sec);
    if (is_output_failed) {
      OnOutputDataFailed();
    } else {
      OnOutputDataOK();
    }
  }

  /*
    Send the update
  */

  if (IsActive()) {
    if (status_tick_ <= up_time) {
      status_tick_ = up_time + Config::report_delay_sec;  // update status timestamp
      if (client_) {
        client_->OnTimeoutUpdated(this);
      }
    }
  }

  return TRUE;
}

gboolean IBaseStream::HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  UNUSED(bus);

  GstMessageType type = GST_MESSAGE_TYPE(message);
  GstObject* src = GST_MESSAGE_SRC(message);
  if (type == GST_MESSAGE_BUFFERING) {
    HandleBufferingMessage(message);
  } else if (type == GST_MESSAGE_CLOCK_LOST) {
    GstClock* clock = nullptr;
    gst_message_parse_clock_lost(message, &clock);
    DEBUG_LOG() << "ASYNC GST_MESSAGE_CLOCK_LOST: " << GST_OBJECT_NAME(clock);
  } else if (type == GST_MESSAGE_DURATION_CHANGED) {
    input_t input = config_->GetUrl();
    for (size_t i = 0; i < input.size(); ++i) {
      auto input_url = input[i].GetUrl();
      if (input_url.SchemeIsHTTPOrHTTPS()) {
        GstBaseSrc* basesrc = reinterpret_cast<GstBaseSrc*>(src);
        UpdateInputProbeStats(probe_in_[i], basesrc->segment.duration);
      }
    }
    DEBUG_LOG() << "ASYNC GST_MESSAGE_DURATION_CHANGED: " << GST_OBJECT_NAME(src);
  } else if (type == GST_MESSAGE_STATE_CHANGED) {
    GstObject* pipeline = GST_OBJECT(pipeline_);
    if (src == pipeline) {
      GstState old_state, new_state, pending_state;
      gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
      if (new_state == GST_STATE_NULL) {
        SetStatus(INIT);
      } else if (new_state == GST_STATE_READY) {
        SetStatus(READY);
      } else if (new_state == GST_STATE_PLAYING) {
        SetStatus(PLAYING);
      }
    }
  } else if (type == GST_MESSAGE_EOS) {
    WARNING_LOG() << " Received end of stream: " << GST_OBJECT_NAME(src);
    if (client_) {
      client_->OnPipelineEOS(this);
    }
  }

  if (client_) {
    client_->OnASyncMessageReceived(this, message);
  }

  return TRUE;
}

void IBaseStream::SetStatus(StreamStatus status) {
  stats_->status = status;
  INFO_LOG() << "Changing status to: " << common::ConvertToString(status);
  if (client_) {
    client_->OnStatusChanged(this, status);
  }
}

bool IBaseStream::IsActive() const {
  return stats_->status != INIT;
}

bool IBaseStream::IsVod() const {
  return GetType() == fastotv::VOD_ENCODE || GetType() == fastotv::VOD_RELAY;
}

void IBaseStream::HandleInputProbeEvent(InputProbe* probe, GstEvent* event) {
  if (client_) {
    client_->OnInputProbeEvent(this, probe, event);
  }
}

void IBaseStream::HandleOutputProbeEvent(OutputProbe* probe, GstEvent* event) {
  if (probe->GetNeedPush()) {
    GstEventType event_type = GST_EVENT_TYPE(event);
    if ((event_type == GST_EVENT_CUSTOM_UPSTREAM || event_type == GST_EVENT_CUSTOM_DOWNSTREAM) &&
        gst_video_event_is_force_key_unit(event)) {
      gpointer parent_ptr = gst_pad_get_parent(probe->GetPad());
      if (parent_ptr) {
        GstElement* parent = GST_ELEMENT_CAST(parent_ptr);
        if (GST_IS_ELEMENT(parent)) {
          const std::string element_plugin_name = elements::Element::GetPluginName(parent);
          if (element_plugin_name == elements::sink::ElementHLSSink::GetPluginName()) {
            elements::sink::ElementHLSSink* hls_sink = new elements::sink::ElementHLSSink("sink", parent);
            const std::string location = hls_sink->GetLocation();
            const common::file_system::ascii_file_string_path fs_template(location);
            delete hls_sink;
            const common::uri::GURL url = probe->GetUrl();
            if (fs_template.IsValid()) {
              if (event_type == GST_EVENT_CUSTOM_UPSTREAM) {
                GstClockTime running_time = 0;
                gboolean all_headers = FALSE;
                guint count = 0;
                if (gst_video_event_parse_upstream_force_key_unit(event, &running_time, &all_headers, &count)) {
                  DEBUG_LOG() << "New keyframe up, running_time: " << running_time << ", all_headers: " << all_headers
                              << ", count: " << count << ", location: " << fs_template.GetParentDirectory()
                              << ", url: " << url.spec();
                }
              } else if (event_type == GST_EVENT_CUSTOM_DOWNSTREAM) {
                GstClockTime timestamp = 0;
                GstClockTime stream_time = 0;
                GstClockTime running_time = 0;
                gboolean all_headers = FALSE;
                guint count = 0;
                if (gst_video_event_parse_downstream_force_key_unit(event, &timestamp, &stream_time, &running_time,
                                                                    &all_headers, &count)) {
                  DEBUG_LOG() << "New keyframe down, timestamp: " << timestamp << ", stream_time: " << stream_time
                              << ", running_time: " << running_time << ", all_headers: " << all_headers
                              << ", count: " << count << ", location: " << fs_template.GetParentDirectory()
                              << ", url: " << url.spec();
                }
              }
            }
          }
        }
        gst_object_unref(parent_ptr);
      }
    }
  }

  if (client_) {
    client_->OnOutputProbeEvent(this, probe, event);
  }
}

elements::Element* IBaseStream::GetElementByName(const std::string& name) const {
  for (elements::Element* el : pipeline_elements_) {
    if (el->GetName() == name) {
      return el;
    }
  }

  NOTREACHED() << "Not founded element name: " << name;
  return nullptr;
}

void IBaseStream::HandleBufferingMessage(GstMessage* message) {
  UNUSED(message);
}

GstPadProbeInfo* IBaseStream::CheckProbeData(InputProbe* probe, GstPadProbeInfo* buff) {
  if (client_) {
    return client_->OnCheckReveivedData(this, probe, buff);
  }

  return buff;
}

GstPadProbeInfo* IBaseStream::CheckProbeDataOutput(OutputProbe* probe, GstPadProbeInfo* buff) {
  if (client_) {
    return client_->OnCheckReveivedOutputData(this, probe, buff);
  }

  return buff;
}

GstStateChangeReturn IBaseStream::SetPipelineState(GstState state) {
  GstStateChangeReturn ret = gst_element_set_state(pipeline_, state);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    GstState current, pending;
    GstStateChangeReturn change_result = gst_element_get_state(pipeline_, &current, &pending, 100000);
    if (change_result == GST_STATE_CHANGE_SUCCESS) {
      WARNING_LOG() << "Unable to change pipeline state from:" << gst_element_state_get_name(current)
                    << " to:" << gst_element_state_get_name(pending);
    }
  }
  return ret;
}

// callbacks

gboolean IBaseStream::main_timer_callback(gpointer user_data) {
  /*
    Restart if no data, set new checkpoint
  */
  IBaseStream* stream = reinterpret_cast<IBaseStream*>(user_data);
  const fastotv::timestamp_t start_ts = common::time::current_utc_mstime();
  gboolean res = stream->HandleMainTimerTick();
  const fastotv::timestamp_t end_ts = common::time::current_utc_mstime();
  DEBUG_LOG() << "HandleMainTimerTick time is: " << end_ts - start_ts << " msec.";
  return res;
}

void IBaseStream::UpdateInputProbeStats(const InputProbe* probe, gsize size) {
  if (probe->GetID() < stats_->input.size()) {
    const size_t prev_total = stats_->input[probe->GetID()].GetTotalBytes();
    stats_->input[probe->GetID()].SetTotalBytes(prev_total + size);
  }
}

void IBaseStream::UpdateOutputProbeStats(const OutputProbe* probe, gsize size) {
  if (probe->GetID() < stats_->output.size()) {
    const size_t prev_total = stats_->output[probe->GetID()].GetTotalBytes();
    stats_->output[probe->GetID()].SetTotalBytes(prev_total + size);
  }
}

const Config* IBaseStream::GetConfig() const {
  return config_;
}

GstBusSyncReply IBaseStream::sync_bus_callback(GstBus* bus, GstMessage* message, gpointer user_data) {
  IBaseStream* stream = reinterpret_cast<IBaseStream*>(user_data);
  return stream->HandleSyncBusMessageReceived(bus, message);
}

gboolean IBaseStream::async_bus_callback(GstBus* bus, GstMessage* message, gpointer user_data) {
  IBaseStream* stream = reinterpret_cast<IBaseStream*>(user_data);
  return stream->HandleAsyncBusMessageReceived(bus, message);
}

bool IBaseStream::DumpIntoFile(const common::file_system::ascii_file_string_path& path) const {
  if (!path.IsValid()) {
    return false;
  }

  std::unique_ptr<dumper::IDumper> dumper(dumper::DumpersFactory::GetInstance().CreateDumper(path));
  if (!dumper) {
    return false;
  }

  return dumper->Dump(GST_BIN(pipeline_), path);
}

}  // namespace stream
}  // namespace fastocloud
