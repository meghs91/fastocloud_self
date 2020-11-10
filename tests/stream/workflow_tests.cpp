/*  Copyright (C) 2014-2019 FastoGT. All right reserved.
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

#include <unistd.h>

#include <thread>

#include <gst/gst.h>

#include <common/file_system/file_system.h>
#include <common/sprintf.h>

#include "stream/configs_factory.h"
#include "stream/link_generator/streamlink.h"
#include "stream/streams/screen_stream.h"
#include "stream/streams_factory.h"

#include "base/config_fields.h"
#include "base/stream_config_parse.h"

#define STREAMS_TRY_COUNT 10

#define SCREEN_SLEEP_SEC 10
#define RELAY_SLEEP_SEC 10
#define RELAY_PLAYLIST_SLEEP_SEC 10
#define ENCODE_SLEEP_SEC 10
#define TIMESHIFT_REC_SLEEP_SEC 10
#define TIMESHIFT_PLAYLIST_SLEEP_SEC 10
#define MOSAIC_SLEEP_SEC 10
#define AD_SLEEP_SEC 10
#define RELAY_AUDIO_SLEEP_SEC 10
#define RELAY_VIDEO_SLEEP_SEC 10
#define STATIC_IMAGE_SLEEP_SEC 10
#define TEST_SLEEP_SEC 10
#define CATCHUP_SLEEP_SEC 20
#define M3U8LOG_SLEEP_SEC 10
#define DIFF_SEC 1

#define YOLOV2_MODEL_GRAPH_PATH PROJECT_TEST_SOURCES_DIR "/data/graph_tinyyolov2_tensorflow.pb"
#define YOLOV3_MODEL_GRAPH_PATH PROJECT_TEST_SOURCES_DIR "/data/graph_tinyyolov3_tensorflow.pb"
#define YOLOV2_LABELS_GRAPH_PATH PROJECT_TEST_SOURCES_DIR "/data/yolov2_labels.txt"
#define YOLOV3_LABELS_GRAPH_PATH PROJECT_TEST_SOURCES_DIR "/data/yolov3_labels.txt"

void* quit_job(fastocloud::stream::IBaseStream* job, uint32_t sleep_sec) {
  static unsigned int seed = time(NULL);
  int rand = rand_r(&seed) % (sleep_sec + DIFF_SEC);
  sleep(rand);
  job->Quit(fastocloud::stream::EXIT_SELF);
  return NULL;
}

void check_encoders() {
  const std::string yuri =
      R"({"urls": [{"id": 115, "uri": "rtsp://admin:admin@192.168.1.168"}]})";
  const std::string enc_uri_str =
      R"({"urls": [{"id": 115, "uri": "tcp://localhost:1935"}]})";
#if defined(MACHINE_LEARNING)
  const std::string deep_learning_str =
      "{\"model_path\": \"" YOLOV2_MODEL_GRAPH_PATH
      "\", \"backend\": 0, \"properties\": [{\"input\":\"input/Placeholder\"}, {\"output\":\"add_8\"}]}";
  const std::string deep_learning_overlay_str = "{\"labels_path\" : \"" YOLOV2_LABELS_GRAPH_PATH "\"}";
#endif

  fastocloud::StreamConfig config_args(new common::HashValue);
  config_args->Insert(ID_FIELD, common::Value::CreateStringValueFromBasicString("encoding"));
  config_args->Insert(TYPE_FIELD, common::Value::CreateIntegerValue(fastotv::ENCODE));
  config_args->Insert(HAVE_AUDIO_FIELD, common::Value::CreateBooleanValue(false));
  config_args->Insert(FEEDBACK_DIR_FIELD, common::Value::CreateStringValueFromBasicString("~"));
  config_args->Insert(VIDEO_CODEC_FIELD, common::Value::CreateStringValueFromBasicString("x264enc"));
  config_args->Insert(AUDIO_CODEC_FIELD, common::Value::CreateStringValueFromBasicString("faac"));
  config_args->Insert(SIZE_FIELD, common::Value::CreateStringValueFromBasicString("416x416"));
  // config_args->Insert(ASPECT_RATIO_FIELD, common::Value::CreateStringValueFromBasicString("3:4"));
  auto ihs = fastocloud::MakeConfigFromJson(yuri);
  config_args->Insert(INPUT_FIELD, ihs.release());
  auto ohs = fastocloud::MakeConfigFromJson(enc_uri_str);
  config_args->Insert(OUTPUT_FIELD, ohs.release());

#if defined(MACHINE_LEARNING)
  auto dep = fastocloud::MakeConfigFromJson(deep_learning_str);
  config_args->Insert(DEEP_LEARNING_FIELD, dep.release());
  config_args->Insert(DEEP_LEARNING_OVERLAY_FIELD, fastocloud::MakeConfigFromJson(deep_learning_overlay_str).release());
#endif

  std::string script_path_str;
  if (!common::file_system::find_file_in_path("streamlink", &script_path_str)) {
    return;
  }

  common::file_system::ascii_file_string_path script_path(script_path_str);
  fastocloud::stream::link_generator::StreamLinkGenerator gena(script_path);
  static const auto test_url = common::uri::Url();

  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    fastocloud::StreamStruct encoding(
        fastocloud::StreamInfo{common::MemSPrintf("encoding_%llu", i), fastotv::ENCODE, {fastocloud::InputUri(0, test_url)}, {fastocloud::OutputUri(0, test_url)}});
    fastocloud::stream::TimeShiftInfo tinf;
    fastocloud::stream::Config* lconfig = nullptr;
    common::Error err = fastocloud::stream::make_config(config_args, &lconfig);
    if (!err) {
      const std::unique_ptr<fastocloud::stream::Config> config_copy(
          fastocloud::stream::make_config_copy(lconfig, &gena));
      fastocloud::stream::IBaseStream* job_enc = fastocloud::stream::StreamsFactory::GetInstance().CreateStream(
          config_copy.get(), nullptr, &encoding, tinf, fastocloud::stream::invalid_chunk_index);
      std::thread th(quit_job, job_enc, ENCODE_SLEEP_SEC);
      CHECK(job_enc->GetType() == fastotv::ENCODE);
      job_enc->Exec();
      th.join();
      delete job_enc;
      delete lconfig;
    }
  }
}

int main(int argc, char** argv) {
  fastocloud::stream::streams_init(argc, argv);
  check_encoders();
  return 0;
}
