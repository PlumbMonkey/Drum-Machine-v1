#ifndef MIDI_FILE_MANAGER_H
#define MIDI_FILE_MANAGER_H

#include <string>
#include <vector>
#include <cstdint>

namespace DrumMachine {

class Pattern;

/**
 * MidiFileManager
 * 
 * Import/Export Standard MIDI Files (.mid)
 * Converts between Pattern step data and MIDI file format.
 * Preserves timing, velocity, and track information.
 * 
 * Milestone 5b: MIDI File I/O
 */

struct MidiEvent {
    enum class Type {
        NOTE_ON,
        NOTE_OFF,
        CONTROL_CHANGE,
        PROGRAM_CHANGE,
        META_SET_TEMPO,
        META_TIME_SIGNATURE,
        META_TRACK_NAME,
        META_END_OF_TRACK
    };

    Type type;
    uint32_t time;           // Delta time in ticks
    uint8_t channel;         // 0-15
    uint8_t note;            // 0-127
    uint8_t velocity;        // 0-127
    uint8_t controller;      // CC controller number
    uint8_t value;           // CC value or program
    uint32_t tempo;          // Microseconds per beat
    std::string trackName;   // For META events
};

class MidiFileManager {
public:
    MidiFileManager();

    // Export pattern to MIDI file
    bool exportToMidi(const std::string& filePath, const Pattern& pattern, 
                      float tempo = 120.0f, uint32_t timeDivision = 480);

    // Import MIDI file to pattern
    bool importFromMidi(const std::string& filePath, Pattern& pattern, 
                        uint32_t targetTrack = 0);

    // Get list of notes in a MIDI file (for preview)
    std::vector<MidiEvent> getMidiEvents(const std::string& filePath);

private:
    // MIDI file parsing helpers
    uint32_t readVariableLength(const uint8_t* data, size_t& offset);
    void writeVariableLength(uint32_t value, std::vector<uint8_t>& data);
    
    uint16_t bytesToUint16(const uint8_t* data);
    uint32_t bytesToUint32(const uint8_t* data);
    
    void writeUint16(uint16_t value, std::vector<uint8_t>& data);
    void writeUint32(uint32_t value, std::vector<uint8_t>& data);

    // Convert MIDI note number to drum track index
    uint32_t midiNoteToTrackIndex(uint8_t midiNote);
    
    // Convert track index to MIDI note number
    uint8_t trackIndexToMidiNote(uint32_t trackIndex);
};

} // namespace DrumMachine

#endif // MIDI_FILE_MANAGER_H
