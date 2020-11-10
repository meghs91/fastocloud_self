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

#include <gst/gstevent.h>

#include <string>
#include <vector>

#include <common/file_system/path.h>
#include <common/types.h>

#if defined(MACHINE_LEARNING)
#include <fastotv/commands_info/ml/types.h>
#endif

#include "base/input_uri.h"
#include "base/stream_struct.h"  // for StreamStatus, StreamStruct (ptr only)

#include "stream/gst_types.h"
#include "stream/ibase_builder_observer.h"

namespace fastocloud {
namespace stream {

class IBaseBuilder;
class InputProbe;
class OutputProbe;
class Config;

enum ExitStatus { EXIT_SELF, EXIT_INNER };

class IBaseStream : public common::IMetaClassInfo, public IBaseBuilderObserver {
 public:
  class IStreamClient {
   public:
    virtual void OnStatusChanged(IBaseStream* stream, StreamStatus status) = 0;
    virtual void OnPipelineEOS(IBaseStream* stream) = 0;
    virtual void OnTimeoutUpdated(IBaseStream* stream) = 0;
    virtual void OnInputProbeEvent(IBaseStream* stream, InputProbe* probe, GstEvent* event) = 0;
    virtual void OnOutputProbeEvent(IBaseStream* stream, OutputProbe* probe, GstEvent* event) = 0;
    virtual void OnSyncMessageReceived(IBaseStream* stream, GstMessage* message) = 0;
    virtual void OnASyncMessageReceived(IBaseStream* stream, GstMessage* message) = 0;
    virtual GstPadProbeInfo* OnCheckReveivedOutputData(IBaseStream* stream,
                                                       OutputProbe* probe,
                                                       GstPadProbeInfo* info) = 0;
    virtual GstPadProbeInfo* OnCheckReveivedData(IBaseStream* stream, InputProbe* probe, GstPadProbeInfo* info) = 0;
    virtual void OnInputChanged(IBaseStream* stream, const InputUri& uri) = 0;
    virtual void OnPipelineCreated(IBaseStream* stream) = 0;
#if defined(MACHINE_LEARNING)
    virtual void OnMlNotification(IBaseStream* stream,
                                  const std::vector<fastotv::commands_info::ml::ImageBox>& images) = 0;
#endif
    virtual ~IStreamClient();
  };

  enum InitedFlag { INITED_NOTHING = 0x0000, INITED_VIDEO = 0x0001, INITED_AUDIO = 0x0002 };
  enum {
    main_timer_msecs = 1000,
    no_data_panic_sec = 60,
    src_timeout_sec = no_data_panic_sec * 2,
    cleanup_life_period_sec = 15 * 2 * 10  // 2x15 sec and 10 chunks
  };

  // channel_id_t not empty
  IBaseStream(const Config* config, IStreamClient* client, StreamStruct* stats);
  const char* ClassName() const override;
  ~IBaseStream() override;

  ExitStatus Exec();
  void Restart();

  bool IsLive() const;

  void Quit(ExitStatus status);
  StreamStruct* GetStats() const;

  time_t GetElipsedTime() const;  // stream life time sec

  fastotv::StreamType GetType() const;

  bool IsActive() const;

  bool IsVod() const;

  virtual void HandleInputProbeEvent(InputProbe* probe, GstEvent* event);
  virtual void HandleOutputProbeEvent(OutputProbe* probe, GstEvent* event);

  virtual GstPadProbeInfo* CheckProbeData(InputProbe* probe, GstPadProbeInfo* buff);
  virtual GstPadProbeInfo* CheckProbeDataOutput(OutputProbe* probe, GstPadProbeInfo* buff);

  void UpdateInputProbeStats(const InputProbe* probe, gsize size);
  void UpdateOutputProbeStats(const OutputProbe* probe, gsize size);

  const Config* GetConfig() const;

  void LinkInputPad(GstPad* pad, element_id_t id, const common::uri::GURL& url);
  void LinkOutputPad(GstPad* pad, element_id_t id, const common::uri::GURL& url, bool need_push);

  size_t CountInputEOS() const;
  size_t CountOutEOS() const;

  bool DumpIntoFile(const common::file_system::ascii_file_string_path& path) const;

 protected:
  elements::Element* GetElementByName(const std::string& name) const;

  bool IsAudioInited() const;
  bool IsVideoInited() const;

  void RegisterAudioCaps(SupportedAudioCodec saudio, GstCaps* caps, element_id_t id);
  void RegisterVideoCaps(SupportedVideoCodec svideo, GstCaps* caps, element_id_t id);

  void SetAudioInited(bool val);
  void SetVideoInited(bool val);

  void OnInpudSrcPadCreated(pad::Pad* src_pad, element_id_t id, const common::uri::GURL& url) override = 0;
  void OnOutputSinkPadCreated(pad::Pad* sink_pad,
                              element_id_t id,
                              const common::uri::GURL& url,
                              bool need_push) override = 0;

  virtual IBaseBuilder* CreateBuilder() = 0;

  virtual void PreLoop() = 0;
  virtual void PostLoop(ExitStatus status) = 0;

  virtual void HandleBufferingMessage(GstMessage* message);

  void SetStatus(StreamStatus status);

  virtual gboolean HandleMainTimerTick();
  virtual GstBusSyncReply HandleSyncBusMessageReceived(GstBus* bus, GstMessage* message);
  virtual gboolean HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message);

  virtual void OnInputDataFailed();
  virtual void OnInputDataOK();

  virtual void OnOutputDataFailed();
  virtual void OnOutputDataOK();

  virtual void PreExecCleanup(time_t old_life_time);
  virtual void PostExecCleanup();

  GstStateChangeReturn SetPipelineState(GstState state);

  IStreamClient* const client_;

  // pipeline actions
  void Stop();
  void Pause();
  void Play();

 private:
  const Config* const config_;

  std::vector<InputProbe*> probe_in_;
  std::vector<OutputProbe*> probe_out_;

  bool InitPipeLine();
  void ClearOutProbes();
  void ClearInProbes();
  void ResetDataWait();

  static GstBusSyncReply sync_bus_callback(GstBus* bus, GstMessage* message, gpointer user_data);
  static gboolean main_timer_callback(gpointer user_data);
  static gboolean async_bus_callback(GstBus* bus, GstMessage* message, gpointer user_data);

  //! Gstreamer loop pointer. You set it up with you custom run-loop.
  GMainLoop* const loop_;
  GstElement* pipeline_;
  elements_line_t pipeline_elements_;

  time_t status_tick_;
  time_t no_data_panic_tick_;

  StreamStruct* const stats_;

  ExitStatus last_exit_status_;
  bool is_live_;

  int flags_;
  int desire_flags_;

  DISALLOW_COPY_AND_ASSIGN(IBaseStream);
};

}  // namespace stream
}  // namespace fastocloud
