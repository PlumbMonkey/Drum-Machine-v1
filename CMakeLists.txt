cmake_minimum_required(VERSION 3.20)
project(DrumMachine VERSION 0.1.0 LANGUAGES CXX)

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

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# External dependencies (header-only or to be fetched)
# These will be added as FetchContent or system packages

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

# Link OpenGL
target_link_libraries(DrumMachine PRIVATE OpenGL::OpenGL)

# TODO: Link RtAudio, SDL2, Dear ImGui, nlohmann/json

# Tests
enable_testing()
add_subdirectory(tests)
