cmake_minimum_required(VERSION 3.15)

project(sq_decoder VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SMTG_VST3_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/vst3sdk" CACHE PATH "Path to the VST3 SDK")

if (NOT EXISTS "${SMTG_VST3_SDK_PATH}/CMakeLists.txt")
  message(FATAL_ERROR "VST3 SDK not found at ${SMTG_VST3_SDK_PATH}. Clone https://github.com/steinbergmedia/vst3sdk into external/vst3sdk or set SMTG_VST3_SDK_PATH.")
endif ()

add_subdirectory(${SMTG_VST3_SDK_PATH} ${CMAKE_BINARY_DIR}/vst3sdk)

smtg_add_vst3plugin(SQDecoder
  source/sqdecoder_entry.cpp
  source/sqdecoder_processor.cpp
  source/sqdecoder_controller.cpp
)

target_include_directories(SQDecoder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

if (COMMAND smtg_set_bundle_name)
  smtg_set_bundle_name(SQDecoder "SQ Decoder")
endif ()

if (COMMAND smtg_set_binary_name)
  smtg_set_binary_name(SQDecoder "SQDecoder")
endif ()

if (COMMAND smtg_set_release_target)
  smtg_set_release_target(SQDecoder)
endif ()
