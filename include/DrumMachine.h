#ifndef DRUM_MACHINE_H
#define DRUM_MACHINE_H

#include <cstdint>
#include <string>
#include <vector>

// Core version
constexpr const char* DRUM_MACHINE_VERSION = "0.1.0";

// Forward declarations
namespace DrumMachine {
    class AudioEngine;
    class Sequencer;
    class Pattern;
    class Transport;
    class UI;
    class DataManager;
}

#endif // DRUM_MACHINE_H
