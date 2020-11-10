SET(GLOBAL_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/gst_macros.h
  ${CMAKE_SOURCE_DIR}/src/stream/gst_types.h
  ${CMAKE_SOURCE_DIR}/src/stream/stypes.h
  ${CMAKE_SOURCE_DIR}/src/stream/commands_factory.h

  ${CMAKE_SOURCE_DIR}/src/stream/ilinker.h

  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder_observer.h
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/probes.h
  ${CMAKE_SOURCE_DIR}/src/stream/timeshift.h
  ${CMAKE_SOURCE_DIR}/src/stream/stream_controller.h
  ${CMAKE_SOURCE_DIR}/src/stream/stream_server.h

  ${CMAKE_SOURCE_DIR}/src/stream/stream_wrapper.h
  ${CMAKE_SOURCE_DIR}/src/stream/gstreamer_init.h
  ${CMAKE_SOURCE_DIR}/src/stream/gstreamer_utils.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams_factory.h
  ${CMAKE_SOURCE_DIR}/src/stream/configs_factory.h
  ${CMAKE_SOURCE_DIR}/src/stream/config.h
)
SET(GLOBAL_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/gst_types.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/stypes.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/commands_factory.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/ilinker.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder_observer.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/probes.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/timeshift.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/stream_controller.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/stream_server.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/stream_wrapper.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/gstreamer_init.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/gstreamer_utils.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/configs_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/config.cpp
)

SET(STREAM_CONFIGS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/relay_config.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/encode_config.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/audio_video_config.h
)

SET(STREAM_CONFIGS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/relay_config.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/encode_config.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/audio_video_config.cpp
)

SET(STREAM_BUILDERS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/gst_base_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/mosaic_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/screen_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/src_decodebin_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay/relay_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay/rtsp_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay/playlist_relay_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/encoding_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/encoding_only_audio_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/encoding_only_video_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/playlist_encoding_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/device_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/rtsp_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/fake_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift/catchup_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift/timeshift_player_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift/timeshift_recorder_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/test/test_life_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/test/test_input_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/display/display_input_stream_builder.h
)
SET(STREAM_BUILDERS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/gst_base_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/mosaic_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/screen_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/src_decodebin_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay/relay_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay/rtsp_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay/playlist_relay_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/encoding_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/encoding_only_audio_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/encoding_only_video_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/playlist_encoding_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/device_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/rtsp_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding/fake_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift/catchup_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift/timeshift_player_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift/timeshift_recorder_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/test/test_life_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/test/test_input_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/display/display_input_stream_builder.cpp
)

SET(STREAMS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_options.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/screen_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/src_decodebin_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/vod/vod_relay_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay/relay_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay/rtsp_relay_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay/playlist_relay_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/vod/vod_encoding_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/encoding_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/encoding_only_audio_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/encoding_only_video_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/playlist_encoding_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/device_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/rtsp_encoding_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/fake_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/catchup_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/timeshift_player_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/itimeshift_recorder_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/timeshift_recorder_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/test/test_life_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/test/test_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/display/display_stream.h

  ${STREAM_BUILDERS_HEADERS}
  ${STREAM_CONFIGS_HEADERS}
)
SET(STREAMS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_options.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/screen_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/src_decodebin_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/vod/vod_relay_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay/relay_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay/rtsp_relay_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay/playlist_relay_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/vod/vod_encoding_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/encoding_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/encoding_only_audio_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/encoding_only_video_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/playlist_encoding_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/device_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/rtsp_encoding_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding/fake_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/catchup_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/timeshift_player_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/itimeshift_recorder_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift/timeshift_recorder_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/test/test_life_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/test/test_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/display/display_stream.cpp

  ${STREAM_BUILDERS_SOURCES}
  ${STREAM_CONFIGS_SOURCES}
)

SET(ELEMENTS_SOURCES_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/element.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/multifilesrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/httpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/dvbsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/appsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/rtmpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/rtspsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/udpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/tcpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/srtsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/v4l2src.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/alsasrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/filesrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/build_input.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/sources.h
)
SET(ELEMENTS_SOURCES_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/element.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/multifilesrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/httpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/dvbsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/appsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/rtmpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/rtspsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/udpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/tcpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/srtsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/v4l2src.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/alsasrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/filesrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/build_input.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/sources.cpp
)

SET(ELEMENTS_SINKS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/rtmp.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/udp.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/file.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/tcp.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/srt.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/http.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/fake.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/test.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/screen.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/build_output.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/sink.h
)
SET(ELEMENTS_SINKS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/rtmp.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/udp.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/file.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/tcp.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/srt.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/http.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/fake.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/test.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/screen.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/build_output.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/sink.cpp
)

SET(ELEMENTS_ENCODERS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/audio.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/video.h
)
SET(ELEMENTS_ENCODERS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/audio.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/video.cpp
)

SET(ELEMENTS_PARSER_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/video.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/audio.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/parser.h
)
SET(ELEMENTS_PARSER_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/video.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/audio.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/parser.cpp
)

SET(ELEMENTS_PAY_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/pay.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/video.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/audio.h
)
SET(ELEMENTS_PAY_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/pay.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/video.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/audio.cpp
)

SET(ELEMENTS_DEPAY_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/depay/depay.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/depay/video.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/depay/audio.h
)
SET(ELEMENTS_DEPAY_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/depay/depay.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/depay/video.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/depay/audio.cpp
)

IF(MACHINE_LEARNING AND FASTOML_FOUND)
  SET(ELEMENTS_DEEP_LEARNING_HEADERS
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/video_ml_filter.h
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/tinyyolov2.h
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/tinyyolov3.h
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/detectionoverlay.h
  )
  SET(ELEMENTS_DEEP_LEARNING_SOURCES
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/video_ml_filter.cpp
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/tinyyolov2.cpp
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/tinyyolov3.cpp
    ${CMAKE_SOURCE_DIR}/src/stream/elements/machine_learning/detectionoverlay.cpp
  )
ENDIF(MACHINE_LEARNING AND FASTOML_FOUND)

SET(ELEMENTS_VIDEO_HEADERS ${CMAKE_SOURCE_DIR}/src/stream/elements/video/video.h)
SET(ELEMENTS_VIDEO_SOURCES ${CMAKE_SOURCE_DIR}/src/stream/elements/video/video.cpp)

SET(ELEMENTS_AUDIO_HEADERS ${CMAKE_SOURCE_DIR}/src/stream/elements/audio/audio.h)
SET(ELEMENTS_AUDIO_SOURCES ${CMAKE_SOURCE_DIR}/src/stream/elements/audio/audio.cpp)

SET(ELEMENTS_MUXER_HEADERS ${CMAKE_SOURCE_DIR}/src/stream/elements/muxer/muxer.h)
SET(ELEMENTS_MUXER_SOURCES ${CMAKE_SOURCE_DIR}/src/stream/elements/muxer/muxer.cpp)

SET(ELEMENTS_HEADERS
  ${ELEMENTS_DEEP_LEARNING_HEADERS}
  ${ELEMENTS_MUXER_HEADERS}
  ${ELEMENTS_PAY_HEADERS}
  ${ELEMENTS_DEPAY_HEADERS}
  ${ELEMENTS_PARSER_HEADERS}
  ${ELEMENTS_ENCODERS_HEADERS}
  ${ELEMENTS_SINKS_HEADERS}
  ${ELEMENTS_SOURCES_HEADERS}
  ${ELEMENTS_VIDEO_HEADERS}
  ${ELEMENTS_AUDIO_HEADERS}
)
SET(ELEMENTS_SOURCES
  ${ELEMENTS_DEEP_LEARNING_SOURCES}
  ${ELEMENTS_MUXER_SOURCES}
  ${ELEMENTS_PAY_SOURCES}
  ${ELEMENTS_DEPAY_SOURCES}
  ${ELEMENTS_PARSER_SOURCES}
  ${ELEMENTS_ENCODERS_SOURCES}
  ${ELEMENTS_SINKS_SOURCES}
  ${ELEMENTS_SOURCES_SOURCES}
  ${ELEMENTS_VIDEO_SOURCES}
  ${ELEMENTS_AUDIO_SOURCES}
)

SET(PAD_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/pad/pad.h
)
SET(PAD_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/pad/pad.cpp
)

SET(DUMPERS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/idumper.h
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/htmldump.h
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/dumpers_factory.h
)
SET(DUMPERS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/htmldump.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/dumpers_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/idumper.cpp
)

SET(LINK_GENERATOR_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/link_generator/ilink_generator.h
  ${CMAKE_SOURCE_DIR}/src/stream/link_generator/streamlink.h
)
SET(LINK_GENERATOR_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/link_generator/ilink_generator.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/link_generator/streamlink.cpp
)

FIND_PACKAGE(GLIB REQUIRED gobject)
FIND_PACKAGE(Gstreamer 1.14.1 REQUIRED)
FIND_PACKAGE(Cairo REQUIRED)

IF(OS_WINDOWS)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES ws2_32)
ELSEIF(OS_MACOSX)
  FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES ${FOUNDATION_LIBRARY})
ELSEIF(OS_POSIX)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES)
  FIND_PACKAGE(X11 REQUIRED)
    IF(X11_FOUND)
      SET(PLATFORM_LIBRARIES ${PLATFORM_LIBRARIES} ${X11_LIBRARIES})
      SET(DEPENDENS_INCLUDE_DIRS ${DEPENDENS_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
      SET(PRIVATE_COMPILE_DEFINITIONS_CORE ${PRIVATE_COMPILE_DEFINITIONS_CORE} HAVE_X11)
  ENDIF(X11_FOUND)
ENDIF(OS_WINDOWS)

IF(USE_PTHREAD)
  IF(NOT OS_ANDROID)
   SET(PLATFORM_LIBRARIES ${PLATFORM_LIBRARIES} pthread)
  ENDIF(NOT OS_ANDROID)
ENDIF(USE_PTHREAD)

SET(CLIENT_SOURCES
  ${GLOBAL_HEADERS} ${GLOBAL_SOURCES}
  ${STREAMS_HEADERS} ${STREAMS_SOURCES}
  ${ELEMENTS_HEADERS} ${ELEMENTS_SOURCES}
  ${PAD_HEADERS} ${PAD_SOURCES}
  ${PLATFORM_HEADER} ${PLATFORM_SOURCES}
  ${DUMPERS_HEADERS} ${DUMPERS_SOURCES}
  ${LINK_GENERATOR_HEADERS} ${LINK_GENERATOR_SOURCES}
)

SET(CLIENT_LIBRARIES
  ${CLIENT_LIBRARIES}
  ${GLIB_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES}
  ${GSTREAMER_LIBRARIES} ${GSTREAMER_APP_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY}
  ${CAIRO_LIBRARIES}
  ${FASTOML_LIBRARIES}
  ${COMMON_LIBRARIES}
  ${STREAMER_COMMON}
  ${PLATFORM_LIBRARIES}
)

SET(PRIVATE_COMPILE_DEFINITIONS_CORE
  ${PRIVATE_COMPILE_DEFINITIONS_CORE}
)
SET(PRIVATE_INCLUDE_DIRECTORIES_CORE
  ${PRIVATE_INCLUDE_DIRECTORIES_CORE}
  ${CMAKE_SOURCE_DIR}/src
  ${GSTREAMER_INCLUDE_DIR}
  ${GLIB_INCLUDE_DIR}
  ${GLIBCONFIG_INCLUDE_DIR}
  ${CAIRO_INCLUDE_DIRS}
  ${DEPENDENS_INCLUDE_DIRS}
  # ${JSONC_INCLUDE_DIRS}
  ${COMMON_INCLUDE_DIRS}
)

ADD_LIBRARY(${STREAMER_CORE} SHARED ${CLIENT_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${STREAMER_CORE} PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_CORE})
TARGET_COMPILE_DEFINITIONS(${STREAMER_CORE} PRIVATE ${PRIVATE_COMPILE_DEFINITIONS_CORE})
TARGET_LINK_LIBRARIES(${STREAMER_CORE} ${CLIENT_LIBRARIES})

IF(PROJECT_BUILD_TYPE_VERSION STREQUAL "release")
  STRIP_TARGET(${STREAMER_CORE})
ENDIF(PROJECT_BUILD_TYPE_VERSION STREQUAL "release")
INSTALL(TARGETS ${STREAMER_CORE} DESTINATION ${LIB_INSTALL_DESTINATION} COMPONENT APPLICATIONS)

IF (DEVELOPER_CHECK_STYLE)
  SET(CHECK_SOURCES ${CLIENT_SOURCES})
  REGISTER_CHECK_STYLE_TARGET(check_style_${STREAMER_CORE} "${CHECK_SOURCES}")
  REGISTER_CHECK_INCLUDES_TARGET(${STREAMER_CORE})
ENDIF(DEVELOPER_CHECK_STYLE)

IF(DEVELOPER_ENABLE_TESTS)
  FIND_PACKAGE(GTest REQUIRED)
  ## Unit tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS}
    ${CMAKE_SOURCE_DIR}/src
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  SET(UNIT_TESTS_LIBS
      ${GTEST_BOTH_LIBRARIES}
      ${STREAMER_COMMON}
      ${PLATFORM_LIBRARIES} ${STREAMER_CORE})
  SET(UNIT_TESTS unit_tests_stream)
  ADD_EXECUTABLE(${UNIT_TESTS}
    ${CMAKE_SOURCE_DIR}/tests/stream/unit_test_link_gen.cpp
    ${CMAKE_SOURCE_DIR}/tests/stream/unit_test_types.cpp
    ${CMAKE_SOURCE_DIR}/tests/stream/unit_test_api.cpp
  )
  TARGET_INCLUDE_DIRECTORIES(${UNIT_TESTS} PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS} ${JSONC_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${UNIT_TESTS} ${UNIT_TESTS_LIBS})
  ADD_TEST_TARGET(${UNIT_TESTS})
  SET_PROPERTY(TARGET ${UNIT_TESTS} PROPERTY FOLDER "Unit tests")

  ## Mock tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_MOCK_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_MOCK_TESTS}
    ${CMAKE_SOURCE_DIR}/src
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  SET(GMOCK_TESTS_LIBS ${GTEST_BOTH_LIBRARIES} gmock ${STREAMER_CORE})
  ADD_EXECUTABLE(gmock_tests
    ${CMAKE_SOURCE_DIR}/tests/stream/mock_test_job.cpp
  )
  TARGET_INCLUDE_DIRECTORIES(gmock_tests PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_MOCK_TESTS})
  TARGET_LINK_LIBRARIES(gmock_tests ${GMOCK_TESTS_LIBS})
  ADD_TEST_TARGET(gmock_tests)
  SET_PROPERTY(TARGET gmock_tests PROPERTY FOLDER "Mock tests")

  # Workflow tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_WORKFLOW_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_WORKFLOW_TESTS}
    ${CMAKE_SOURCE_DIR}/src
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  SET(WORKFLOW_TESTS_LIBS ${STREAMER_CORE})
  ADD_EXECUTABLE(workflow_tests ${CMAKE_SOURCE_DIR}/tests/stream/workflow_tests.cpp)
  TARGET_INCLUDE_DIRECTORIES(workflow_tests PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_WORKFLOW_TESTS})
  TARGET_COMPILE_DEFINITIONS(workflow_tests PRIVATE -DPROJECT_TEST_SOURCES_DIR="${CMAKE_SOURCE_DIR}/tests")
  TARGET_LINK_LIBRARIES(workflow_tests ${WORKFLOW_TESTS_LIBS})
  SET_PROPERTY(TARGET workflow_tests PROPERTY FOLDER "Workflow tests")
ENDIF(DEVELOPER_ENABLE_TESTS)
