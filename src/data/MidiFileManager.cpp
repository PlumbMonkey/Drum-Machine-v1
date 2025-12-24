#include "MidiFileManager.h"
#include "../sequencer/Pattern.h"
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace DrumMachine {

MidiFileManager::MidiFileManager() {
}

uint32_t MidiFileManager::readVariableLength(const uint8_t* data, size_t& offset) {
    uint32_t value = 0;
    uint8_t byte;
    
    do {
        if (offset >= 0xFFFFFFFF) return 0; // Safety check
        byte = data[offset++];
        value = (value << 7) | (byte & 0x7F);
    } while (byte & 0x80);
    
    return value;
}

void MidiFileManager::writeVariableLength(uint32_t value, std::vector<uint8_t>& data) {
    std::vector<uint8_t> bytes;
    bytes.push_back(value & 0x7F);
    
    while ((value >>= 7) > 0) {
        bytes.push_back((value & 0x7F) | 0x80);
    }
    
    for (auto it = bytes.rbegin(); it != bytes.rend(); ++it) {
        data.push_back(*it);
    }
}

uint16_t MidiFileManager::bytesToUint16(const uint8_t* data) {
    return (static_cast<uint16_t>(data[0]) << 8) | static_cast<uint16_t>(data[1]);
}

uint32_t MidiFileManager::bytesToUint32(const uint8_t* data) {
    return (static_cast<uint32_t>(data[0]) << 24) |
           (static_cast<uint32_t>(data[1]) << 16) |
           (static_cast<uint32_t>(data[2]) << 8) |
           static_cast<uint32_t>(data[3]);
}

void MidiFileManager::writeUint16(uint16_t value, std::vector<uint8_t>& data) {
    data.push_back((value >> 8) & 0xFF);
    data.push_back(value & 0xFF);
}

void MidiFileManager::writeUint32(uint32_t value, std::vector<uint8_t>& data) {
    data.push_back((value >> 24) & 0xFF);
    data.push_back((value >> 16) & 0xFF);
    data.push_back((value >> 8) & 0xFF);
    data.push_back(value & 0xFF);
}

uint32_t MidiFileManager::midiNoteToTrackIndex(uint8_t midiNote) {
    // Map MIDI notes to drum tracks
    // Drum Kit: C1=36 (kick), D1=38 (snare), E1=40 (hihat), etc.
    // Map to drum tracks 0-7
    if (midiNote >= 36 && midiNote <= 43) {
        return midiNote - 36; // 36-43 → 0-7
    }
    return 0; // Default to track 0
}

uint8_t MidiFileManager::trackIndexToMidiNote(uint32_t trackIndex) {
    // Reverse mapping: track 0-7 → MIDI note 36-43
    return 36 + (trackIndex % 8);
}

bool MidiFileManager::exportToMidi(const std::string& filePath, const Pattern& pattern,
                                   float tempo, uint32_t timeDivision) {
    try {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) return false;

        // MIDI Header
        std::vector<uint8_t> header;
        header.insert(header.end(), {'M', 'T', 'h', 'd'});
        writeUint32(6, header); // Header length
        writeUint16(0, header); // Format 0 (single track)
        writeUint16(1, header); // Number of tracks
        writeUint16(static_cast<uint16_t>(timeDivision), header); // Time division (ticks per quarter note)
        
        file.write(reinterpret_cast<char*>(header.data()), header.size());

        // MIDI Track
        std::vector<uint8_t> trackData;

        // Set Tempo event (microseconds per beat)
        // 60,000,000 / tempo = microseconds per beat
        uint32_t microsecondsPerBeat = static_cast<uint32_t>(60000000.0f / tempo);
        trackData.push_back(0x00); // Delta time = 0
        trackData.push_back(0xFF); // Meta event
        trackData.push_back(0x51); // Set Tempo
        trackData.push_back(0x03); // Length = 3 bytes
        trackData.push_back(static_cast<uint8_t>((microsecondsPerBeat >> 16) & 0xFF));
        trackData.push_back(static_cast<uint8_t>((microsecondsPerBeat >> 8) & 0xFF));
        trackData.push_back(static_cast<uint8_t>(microsecondsPerBeat & 0xFF));

        // Track Name event
        trackData.push_back(0x00); // Delta time
        trackData.push_back(0xFF); // Meta event
        trackData.push_back(0x03); // Sequence/Track Name
        std::string trackName = "Drum Pattern";
        trackData.push_back(static_cast<uint8_t>(trackName.size()));
        trackData.insert(trackData.end(), trackName.begin(), trackName.end());

        // Convert pattern to MIDI events
        uint32_t samplesPerStep = timeDivision; // Each step = 1 beat (480 ticks at 480 ticks/beat)
        uint32_t currentTick = 0;

        for (uint32_t step = 0; step < 16; ++step) {
            for (uint32_t track = 0; track < 8; ++track) {
                if (pattern.isStepActive(track, step)) {
                    uint32_t stepTick = step * samplesPerStep;
                    uint32_t deltaTime = stepTick - currentTick;

                    // Note On
                    writeVariableLength(deltaTime, trackData);
                    trackData.push_back(0x90); // Note On, channel 0
                    trackData.push_back(trackIndexToMidiNote(track)); // Note number
                    trackData.push_back(100); // Velocity

                    currentTick = stepTick;

                    // Note Off (very short duration)
                    writeVariableLength(1, trackData); // Delta time = 1 tick
                    trackData.push_back(0x80); // Note Off, channel 0
                    trackData.push_back(trackIndexToMidiNote(track));
                    trackData.push_back(0); // Release velocity

                    currentTick += 1;
                }
            }
        }

        // End of Track meta event
        uint32_t finalDeltaTime = (16 * samplesPerStep) - currentTick;
        writeVariableLength(finalDeltaTime, trackData);
        trackData.push_back(0xFF); // Meta event
        trackData.push_back(0x2F); // End of Track
        trackData.push_back(0x00); // Length = 0

        // Write track header and data
        std::vector<uint8_t> track;
        track.insert(track.end(), {'M', 'T', 'r', 'k'});
        writeUint32(static_cast<uint32_t>(trackData.size()), track);
        track.insert(track.end(), trackData.begin(), trackData.end());

        file.write(reinterpret_cast<char*>(track.data()), track.size());
        file.close();

        return true;
    } catch (...) {
        return false;
    }
}

bool MidiFileManager::importFromMidi(const std::string& filePath, Pattern& pattern,
                                     uint32_t /* targetTrack */) {
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) return false;

        // Read entire file
        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)),
                                       std::istreambuf_iterator<char>());
        file.close();

        size_t offset = 0;

        // Check MIDI header
        if (fileData.size() < 14 || 
            fileData[0] != 'M' || fileData[1] != 'T' || 
            fileData[2] != 'h' || fileData[3] != 'd') {
            return false;
        }

        offset += 4; // Skip "MThd"
        offset += 4; // Skip header length
        uint16_t format = bytesToUint16(fileData.data() + offset);
        offset += 2;
        uint16_t numTracks = bytesToUint16(fileData.data() + offset);
        offset += 2;
        uint16_t timeDivision = bytesToUint16(fileData.data() + offset);
        offset += 2;

        (void)format; // Unused but parsed for completeness
        if (numTracks == 0) return false;

        // Parse first track
        if (offset + 4 > fileData.size() ||
            fileData[offset] != 'M' || fileData[offset + 1] != 'T' ||
            fileData[offset + 2] != 'r' || fileData[offset + 3] != 'k') {
            return false;
        }

        offset += 4; // Skip "MTrk"
        uint32_t trackLength = bytesToUint32(fileData.data() + offset);
        offset += 4;

        size_t trackEnd = offset + trackLength;
        uint32_t currentTick = 0;

        // Parse MIDI events
        while (offset < trackEnd && offset < fileData.size()) {
            uint32_t deltaTime = readVariableLength(fileData.data(), offset);
            currentTick += deltaTime;

            if (offset >= fileData.size()) break;

            uint8_t status = fileData[offset++];

            // Handle meta events
            if (status == 0xFF) {
                if (offset >= fileData.size()) break;
                offset++; // Skip meta type
                uint32_t length = readVariableLength(fileData.data(), offset);
                offset += length;
                continue;
            }

            // Handle MIDI channel messages
            uint8_t statusType = status & 0xF0;

            if (statusType == 0x90 || statusType == 0x80) { // Note On/Off
                if (offset + 1 >= fileData.size()) break;
                uint8_t note = fileData[offset++];
                uint8_t velocity = fileData[offset++];

                if (statusType == 0x90 && velocity > 0) { // Note On
                    // Convert MIDI note to step index (0-15)
                    // Assuming 16 steps per bar, map tick to step
                    uint32_t step = (currentTick / (static_cast<uint32_t>(timeDivision) / 4)) % 16;

                    if (step < 16) {
                        // Set step active for the appropriate track
                        uint32_t track = midiNoteToTrackIndex(note);
                        pattern.setStepActive(track, step, true);
                    }
                }
            } else if (statusType == 0xB0) { // Control Change
                if (offset + 1 >= fileData.size()) break;
                offset += 2; // Skip CC controller and value
            } else if (statusType == 0xC0) { // Program Change
                if (offset >= fileData.size()) break;
                offset++; // Skip program
            } else if (statusType == 0xE0) { // Pitch Bend
                if (offset + 1 >= fileData.size()) break;
                offset += 2; // Skip pitch bend data
            }
        }

        return true;
    } catch (...) {
        return false;
    }
}

std::vector<MidiEvent> MidiFileManager::getMidiEvents(const std::string& filePath) {
    std::vector<MidiEvent> events;
    
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) return events;

        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)),
                                       std::istreambuf_iterator<char>());
        file.close();

        size_t offset = 0;

        if (fileData.size() < 14 ||
            fileData[0] != 'M' || fileData[1] != 'T' ||
            fileData[2] != 'h' || fileData[3] != 'd') {
            return events;
        }

        offset += 4;
        uint32_t headerLength = bytesToUint32(fileData.data() + offset);
        offset += 4 + headerLength;

        // Parse first track
        if (offset + 8 > fileData.size() ||
            fileData[offset] != 'M' || fileData[offset + 1] != 'T' ||
            fileData[offset + 2] != 'r' || fileData[offset + 3] != 'k') {
            return events;
        }

        offset += 4;
        uint32_t trackLength = bytesToUint32(fileData.data() + offset);
        offset += 4;

        size_t trackEnd = offset + trackLength;
        uint32_t currentTick = 0;

        while (offset < trackEnd && offset < fileData.size()) {
            uint32_t deltaTime = readVariableLength(fileData.data(), offset);
            currentTick += deltaTime;

            if (offset >= fileData.size()) break;

            uint8_t status = fileData[offset++];

            if (status == 0xFF) {
                if (offset >= fileData.size()) break;
                uint8_t metaType = fileData[offset++];
                uint32_t length = readVariableLength(fileData.data(), offset);

                if (metaType == 0x51) { // Set Tempo
                    if (offset + 3 <= fileData.size()) {
                        uint32_t tempo = (static_cast<uint32_t>(fileData[offset]) << 16) |
                                        (static_cast<uint32_t>(fileData[offset + 1]) << 8) |
                                        static_cast<uint32_t>(fileData[offset + 2]);
                        events.push_back({MidiEvent::Type::META_SET_TEMPO, currentTick, 0, 0, 0, 0, 0, tempo, ""});
                    }
                }

                offset += length;
                continue;
            }

            uint8_t statusType = status & 0xF0;
            uint8_t channel = status & 0x0F;

            if (statusType == 0x90 || statusType == 0x80) {
                if (offset + 1 >= fileData.size()) break;
                uint8_t note = fileData[offset++];
                uint8_t velocity = fileData[offset++];

                if (statusType == 0x90) {
                    events.push_back({MidiEvent::Type::NOTE_ON, currentTick, channel, note, velocity, 0, 0, 0, ""});
                } else {
                    events.push_back({MidiEvent::Type::NOTE_OFF, currentTick, channel, note, velocity, 0, 0, 0, ""});
                }
            } else if (statusType == 0xB0) {
                if (offset + 1 >= fileData.size()) break;
                uint8_t controller = fileData[offset++];
                uint8_t value = fileData[offset++];
                events.push_back({MidiEvent::Type::CONTROL_CHANGE, currentTick, channel, 0, 0, controller, value, 0, ""});
            }
        }
    } catch (...) {
    }

    return events;
}

} // namespace DrumMachine
