#ifndef PATTERN_MANAGER_H
#define PATTERN_MANAGER_H

#include "../data/DataManager.h"
#include "../data/MidiFileManager.h"
#include <string>
#include <vector>

namespace DrumMachine {

class Pattern;

/**
 * PatternManager
 * 
 * UI-level pattern persistence with file browser.
 * Manages save/load dialogs and pattern list for JSON and MIDI.
 */
class PatternManager {
public:
    PatternManager();

    // Save current pattern to JSON file
    bool savePattern(const std::string& filename, const Pattern& pattern);

    // Load pattern from JSON file
    bool loadPattern(const std::string& filename, Pattern& pattern);

    // Export pattern to MIDI file
    bool exportToMidi(const std::string& filename, const Pattern& pattern, 
                      float tempo = 120.0f);

    // Import MIDI file to pattern
    bool importFromMidi(const std::string& filename, Pattern& pattern);

    // Get list of available JSON patterns
    std::vector<std::string> getAvailablePatterns() const;

    // Get list of available MIDI files
    std::vector<std::string> getAvailableMidiFiles() const;

    // Get default pattern directory
    static const char* getPatternDirectory() { return "patterns/"; }
    static const char* getMidiDirectory() { return "midi/"; }

private:
    DataManager dataManager_;
    MidiFileManager midiManager_;
};

} // namespace DrumMachine

#endif // PATTERN_MANAGER_H
