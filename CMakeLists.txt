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

# Fetch SDL2 (zlib License)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0
)

# Fetch Dear ImGui (MIT License)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
)

# Fetch RtMidi (MIT License)
FetchContent_Declare(
    rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG master
)

# Disable tests for RtAudio and RtMidi to avoid conflicts
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Make available
FetchContent_MakeAvailable(rtaudio rtmidi nlohmann_json dr_wav SDL2 imgui)

# Get dr_wav and rtaudio source dirs
FetchContent_GetProperties(dr_wav SOURCE_DIR DR_WAV_SOURCE_DIR)
FetchContent_GetProperties(rtaudio SOURCE_DIR RTAUDIO_SOURCE_DIR)
FetchContent_GetProperties(nlohmann_json SOURCE_DIR NLOHMANN_JSON_SOURCE_DIR)
FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_SOURCE_DIR)
FetchContent_GetProperties(SDL2 SOURCE_DIR SDL2_SOURCE_DIR)

# ImGui backend sources
set(IMGUI_SOURCES
    ${IMGUI_SOURCE_DIR}/imgui.cpp
    ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
    ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${DR_WAV_SOURCE_DIR})
include_directories(${RTAUDIO_SOURCE_DIR})
include_directories(${NLOHMANN_JSON_SOURCE_DIR}/include)
include_directories(${IMGUI_SOURCE_DIR})
include_directories(${IMGUI_SOURCE_DIR}/backends)
include_directories(${SDL2_SOURCE_DIR}/include)

FetchContent_GetProperties(rtmidi SOURCE_DIR RTMIDI_SOURCE_DIR)
include_directories(${RTMIDI_SOURCE_DIR})

# Source files
set(AUDIO_SOURCES
    src/audio/AudioEngine.cpp
    src/audio/SamplePlayer.cpp
    src/audio/MidiManager.cpp
)

set(SEQUENCER_SOURCES
    src/sequencer/Sequencer.cpp
    src/sequencer/Pattern.cpp
    src/sequencer/Transport.cpp
)

set(UI_SOURCES
    src/ui/UI.cpp
    src/ui/AssetManager.cpp
    src/ui/Window.cpp
    src/ui/StepEditor.cpp
    src/ui/PatternManager.cpp
    src/ui/SwingVisualizer.cpp
    src/ui/SampleBrowser.cpp
    src/ui/MidiController.cpp
)

set(DATA_SOURCES
    src/data/DataManager.cpp
    src/data/MidiFileManager.cpp
)

set(CORE_SOURCES
    src/core/ParameterBus.cpp
)

# Option to build CLI version instead of GUI
option(BUILD_CLI_ONLY "Build CLI version without UI (no SDL2/OpenGL required)" OFF)

if(BUILD_CLI_ONLY)
    message(STATUS "Building CLI version (no UI)")
    set(MAIN_SOURCE src/main_cli.cpp)
else()
    message(STATUS "Building GUI version with UI")
    set(MAIN_SOURCE src/main.cpp)
endif()

# Main executable
add_executable(DrumMachine
    ${MAIN_SOURCE}
    ${CORE_SOURCES}
    ${AUDIO_SOURCES}
    ${SEQUENCER_SOURCES}
    $<$<NOT:$<BOOL:${BUILD_CLI_ONLY}>>:${UI_SOURCES}>
    $<$<NOT:$<BOOL:${BUILD_CLI_ONLY}>>:${IMGUI_SOURCES}>
    ${DATA_SOURCES}
)

# Link libraries
if(BUILD_CLI_ONLY)
    # CLI version only needs audio libraries
    target_link_libraries(DrumMachine PRIVATE 
        rtaudio
        rtmidi
        nlohmann_json::nlohmann_json
    )
else()
    # GUI version needs UI libraries
    target_link_libraries(DrumMachine PRIVATE 
        rtaudio
        rtmidi
        nlohmann_json::nlohmann_json
        SDL2::SDL2
        OpenGL::GL
    )
endif()

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
