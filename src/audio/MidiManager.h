#ifndef MIDI_MANAGER_H
#define MIDI_MANAGER_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace DrumMachine {

class ParameterBus;

/**
 * MidiManager
 * 
 * Handles MIDI input from external controllers.
 * Runs on separate thread to avoid blocking audio.
 * Publishes MIDI events via ParameterBus for other modules.
 * 
 * Milestone 5: MIDI Foundation
 */

struct MidiMessage {
    enum class Type {
        NOTE_ON,
        NOTE_OFF,
        CONTROL_CHANGE,
        PROGRAM_CHANGE,
        PITCH_BEND,
        UNKNOWN
    };

    Type type;
    uint8_t channel;      // 0-15
    uint8_t note;         // 0-127 (for NOTE_ON/OFF)
    uint8_t velocity;     // 0-127 (for NOTE_ON/OFF)
    uint8_t controller;   // 0-119 (for CC)
    uint8_t value;        // 0-127 (for CC or pitch bend)
    uint64_t timestamp;   // When message was received
};

class MidiManager {
public:
    MidiManager();
    ~MidiManager();

    // Initialize MIDI input
    bool initialize();
    
    // Shutdown MIDI input
    void shutdown();

    // Check if MIDI input is active
    bool isActive() const;

    // Get list of available MIDI input ports
    std::vector<std::string> getInputPorts() const;

    // Open a specific MIDI input port
    bool openPort(uint32_t portIndex);

    // Open virtual MIDI input (for software controllers)
    bool openVirtualPort(const std::string& portName = "Drum Machine Input");

    // Process incoming MIDI messages
    // Returns number of messages processed
    uint32_t processMessages();

    // Callback for MIDI note on
    using MidiCallback = std::function<void(const MidiMessage&)>;
    void setMidiCallback(MidiCallback callback);

    // Get current MIDI CC values (0-127)
    uint8_t getControlValue(uint8_t controller) const;

private:
    class RtMidiWrapper;
    std::unique_ptr<RtMidiWrapper> rtMidiIn_;

    bool isActive_;
    MidiCallback midiCallback_;
    uint8_t controlValues_[120];  // CC value cache

    // Process a single MIDI message
    void processMidiMessage(const MidiMessage& msg);
};

} // namespace DrumMachine

#endif // MIDI_MANAGER_H
