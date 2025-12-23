cmake_minimum_required(VERSION 3.20)
project(DrumMachine VERSION 0.1.0 LANGUAGES CXX)

# Policy for CMake 3.20+
set(CMAKE_POLICY_VERSION_MINIMUM 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific settings
if(WIN32)
    add_compile_options(/W4)
elseif(APPLE)
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(UNIX)
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# FetchContent for external dependencies
include(FetchContent)

# Find required packages
find_package(OpenGL REQUIRED)

# Fetch RtAudio (MIT License)
FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG master
)

# Fetch dr_wav (header-only, Public Domain)
FetchContent_Declare(
    dr_wav
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG master
)

# Fetch nlohmann/json (header-only, MIT License)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

# Make available
FetchContent_MakeAvailable(rtaudio nlohmann_json dr_wav)

# Get dr_wav and rtaudio source dirs
FetchContent_GetProperties(dr_wav SOURCE_DIR DR_WAV_SOURCE_DIR)
FetchContent_GetProperties(rtaudio SOURCE_DIR RTAUDIO_SOURCE_DIR)
FetchContent_GetProperties(nlohmann_json SOURCE_DIR NLOHMANN_JSON_SOURCE_DIR)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${DR_WAV_SOURCE_DIR})
include_directories(${RTAUDIO_SOURCE_DIR})
include_directories(${NLOHMANN_JSON_SOURCE_DIR}/include)

# Source files
set(AUDIO_SOURCES
    src/audio/AudioEngine.cpp
    src/audio/SamplePlayer.cpp
)

set(SEQUENCER_SOURCES
    src/sequencer/Sequencer.cpp
    src/sequencer/Pattern.cpp
    src/sequencer/Transport.cpp
)

set(UI_SOURCES
    src/ui/UI.cpp
    src/ui/AssetManager.cpp
)

set(DATA_SOURCES
    src/data/DataManager.cpp
)

set(CORE_SOURCES
    src/main.cpp
)

# Main executable
add_executable(DrumMachine
    ${CORE_SOURCES}
    ${AUDIO_SOURCES}
    ${SEQUENCER_SOURCES}
    ${UI_SOURCES}
    ${DATA_SOURCES}
)

# Link libraries
target_link_libraries(DrumMachine PRIVATE 
    rtaudio
    nlohmann_json::nlohmann_json
)

# Platform-specific audio API
if(WIN32)
    target_compile_definitions(DrumMachine PRIVATE __WINDOWS_WASAPI__)
elseif(APPLE)
    target_compile_definitions(DrumMachine PRIVATE __MACOSX_CORE__)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(DrumMachine PRIVATE __LINUX_ALSA__)
endif()

# Tests
enable_testing()
add_subdirectory(tests)
