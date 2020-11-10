OPTION(MACHINE_LEARNING "ML plugins" OFF)
OPTION(AMAZON_KINESIS "AWS KVS plugins" OFF)

MESSAGE(STATUS "MACHINE_LEARNING: ${MACHINE_LEARNING}")
MESSAGE(STATUS "AMAZON_KINESIS: ${AMAZON_KINESIS}")

# projects globals names
SET(STREAMER_NAME streamer CACHE STRING "Stream process name")
SET(STREAMER_COMMON ${STREAMER_NAME}_common)

SET(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/src)

# for sources only absolute paths
SET(BASE_HEADERS
  ${CMAKE_SOURCE_DIR}/src/base/types.h
  ${CMAKE_SOURCE_DIR}/src/base/utils.h
  ${CMAKE_SOURCE_DIR}/src/base/stream_config.h
  ${CMAKE_SOURCE_DIR}/src/base/stream_config_parse.h
  ${CMAKE_SOURCE_DIR}/src/base/input_uri.h
  ${CMAKE_SOURCE_DIR}/src/base/output_uri.h
  ${CMAKE_SOURCE_DIR}/src/base/constants.h
  ${CMAKE_SOURCE_DIR}/src/base/gst_constants.h
  ${CMAKE_SOURCE_DIR}/src/base/config_fields.h
  ${CMAKE_SOURCE_DIR}/src/base/logo.h
  ${CMAKE_SOURCE_DIR}/src/base/rsvg_logo.h
  ${CMAKE_SOURCE_DIR}/src/base/inputs_outputs.h
  ${CMAKE_SOURCE_DIR}/src/base/channel_stats.h
  ${CMAKE_SOURCE_DIR}/src/base/stream_info.h
  ${CMAKE_SOURCE_DIR}/src/base/stream_struct.h
)

SET(BASE_SOURCES
  ${CMAKE_SOURCE_DIR}/src/base/types.cpp
  ${CMAKE_SOURCE_DIR}/src/base/utils.cpp
  ${CMAKE_SOURCE_DIR}/src/base/stream_config.cpp
  ${CMAKE_SOURCE_DIR}/src/base/stream_config_parse.cpp
  ${CMAKE_SOURCE_DIR}/src/base/input_uri.cpp
  ${CMAKE_SOURCE_DIR}/src/base/output_uri.cpp
  ${CMAKE_SOURCE_DIR}/src/base/constants.cpp
  ${CMAKE_SOURCE_DIR}/src/base/gst_constants.cpp
  ${CMAKE_SOURCE_DIR}/src/base/config_fields.cpp
  ${CMAKE_SOURCE_DIR}/src/base/logo.cpp
  ${CMAKE_SOURCE_DIR}/src/base/rsvg_logo.cpp
  ${CMAKE_SOURCE_DIR}/src/base/inputs_outputs.cpp
  ${CMAKE_SOURCE_DIR}/src/base/channel_stats.cpp
  ${CMAKE_SOURCE_DIR}/src/base/stream_info.cpp
  ${CMAKE_SOURCE_DIR}/src/base/stream_struct.cpp
)

SET(STREAM_COMMANDS_INFO_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands.h
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_factory.h
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/stop_info.h
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/restart_info.h
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/changed_sources_info.h
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/statistic_info.h
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/details/channel_stats_info.h
)
SET(STREAM_COMMANDS_INFO_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands.cpp
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/stop_info.cpp
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/restart_info.cpp
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/changed_sources_info.cpp
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/statistic_info.cpp
  ${CMAKE_SOURCE_DIR}/src/stream_commands/commands_info/details/channel_stats_info.cpp
)

FIND_PACKAGE(Common REQUIRED)
FIND_PACKAGE(FastoTvCPP REQUIRED)
FIND_PACKAGE(JSON-C REQUIRED)

# options
IF(MACHINE_LEARNING)
  FIND_PACKAGE(FastoML REQUIRED)
  IF (NOT FASTOML_FOUND)
    MESSAGE(SEND_ERROR "Please install ${PROJECT_COMPANYNAME} gstreamer machine learning framework.")
  ENDIF(NOT FASTOML_FOUND)
  ADD_DEFINITIONS(-DMACHINE_LEARNING)

  SET(BASE_HEADERS ${BASE_HEADERS}
    ${CMAKE_SOURCE_DIR}/src/base/machine_learning/deep_learning.h
    ${CMAKE_SOURCE_DIR}/src/base/machine_learning/deep_learning_overlay.h
  )
  SET(BASE_SOURCES ${BASE_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/base/machine_learning/deep_learning.cpp
    ${CMAKE_SOURCE_DIR}/src/base/machine_learning/deep_learning_overlay.cpp
  )
ENDIF(MACHINE_LEARNING)

IF(AMAZON_KINESIS)
  ADD_DEFINITIONS(-DAMAZON_KINESIS)
  SET(BASE_HEADERS ${BASE_HEADERS}
    ${CMAKE_SOURCE_DIR}/src/base/amazon_kinesis/amazon_kinesis.h
  )
  SET(BASE_SOURCES ${BASE_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/base/amazon_kinesis/amazon_kinesis.cpp
  )
ENDIF(AMAZON_KINESIS)

IF(OS_WINDOWS)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES)
ELSEIF(OS_LINUX)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES)
ELSEIF(OS_POSIX)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES)
ENDIF(OS_WINDOWS)

IF(USE_PTHREAD)
  IF(NOT OS_ANDROID)
   SET(PLATFORM_LIBRARIES ${PLATFORM_LIBRARIES} pthread)
  ENDIF(NOT OS_ANDROID)
ENDIF(USE_PTHREAD)

ADD_SUBDIRECTORY(utils)

# common iptv lib

SET(STREAMER_COMMON_SOURCES
  ${STREAM_COMMANDS_INFO_HEADERS} ${STREAM_COMMANDS_INFO_SOURCES}
  ${BASE_HEADERS} ${BASE_SOURCES}
)
SET(STREAMER_COMMON_LIBRARIES
  ${STREAMER_COMMON_LIBRARIES}
  ${JSONC_LIBRARIES} utils
  ${COMMON_BASE_LIBRARY}
  ${FASTOTV_CPP_LIBRARIES}
  ${ZLIB_LIBRARIES}
)
SET(PRIVATE_INCLUDE_DIRECTORIES_COMMON
  ${PRIVATE_INCLUDE_DIRECTORIES_COMMON}
  ${CMAKE_SOURCE_DIR}/src
  ${FASTOTV_CPP_INCLUDE_DIRS}
  ${COMMON_INCLUDE_DIRS}
)

ADD_LIBRARY(${STREAMER_COMMON} STATIC ${STREAMER_COMMON_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${STREAMER_COMMON} PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_COMMON})
TARGET_LINK_LIBRARIES(${STREAMER_COMMON} ${STREAMER_COMMON_LIBRARIES})

SET(STREAMER_CORE ${STREAMER_NAME}_core)

IF(BUILD_SERVER)
  ADD_SUBDIRECTORY(server)
ENDIF(BUILD_SERVER)
IF(BUILD_STREAM)
  ADD_SUBDIRECTORY(stream)
ENDIF(BUILD_STREAM)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION . COMPONENT LICENSE RENAME LICENSE OPTIONAL)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/COPYRIGHT DESTINATION . COMPONENT LICENSE RENAME COPYRIGHT OPTIONAL)
INSTALL(FILES ${PROJECT_CHANGELOG_FILE} DESTINATION . COMPONENT LICENSE RENAME CHANGELOG OPTIONAL)

IF (DEVELOPER_CHECK_STYLE)
  SET(CHECK_SOURCES ${STREAMER_COMMON_SOURCES})
  REGISTER_CHECK_STYLE_TARGET(check_style_${STREAMER_COMMON} "${CHECK_SOURCES}")
  REGISTER_CHECK_INCLUDES_TARGET(${STREAMER_COMMON})
ENDIF(DEVELOPER_CHECK_STYLE)

IF(DEVELOPER_ENABLE_TESTS)
  FIND_PACKAGE(GTest REQUIRED)

  ## Unit tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS}
    ${CMAKE_SOURCE_DIR}/src
  )
  SET(UNIT_TESTS_LIBS
      ${GTEST_BOTH_LIBRARIES}
      ${STREAMER_COMMON}
      ${PLATFORM_LIBRARIES})
  SET(UNIT_TESTS unit_tests)
  ADD_EXECUTABLE(${UNIT_TESTS}
    ${CMAKE_SOURCE_DIR}/tests/unit_test_output_uri.cpp
    ${CMAKE_SOURCE_DIR}/tests/unit_test_input_uri.cpp
    ${CMAKE_SOURCE_DIR}/tests/unit_test_types.cpp
  )
  TARGET_INCLUDE_DIRECTORIES(${UNIT_TESTS} PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS} ${JSONC_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${UNIT_TESTS} ${UNIT_TESTS_LIBS})
  ADD_TEST_TARGET(${UNIT_TESTS})
  SET_PROPERTY(TARGET ${UNIT_TESTS} PROPERTY FOLDER "Unit tests")
ENDIF(DEVELOPER_ENABLE_TESTS)
