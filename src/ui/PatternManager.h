#ifndef PATTERN_MANAGER_H
#define PATTERN_MANAGER_H

#include "../data/DataManager.h"
#include <string>
#include <vector>

namespace DrumMachine {

class Pattern;

/**
 * PatternManager
 * 
 * UI-level pattern persistence with file browser.
 * Manages save/load dialogs and pattern list.
 */
class PatternManager {
public:
    PatternManager();

    // Save current pattern to file
    bool savePattern(const std::string& filename, const Pattern& pattern);

    // Load pattern from file
    bool loadPattern(const std::string& filename, Pattern& pattern);

    // Get list of available patterns
    std::vector<std::string> getAvailablePatterns() const;

    // Get default pattern directory
    static const char* getPatternDirectory() { return "patterns/"; }

private:
    DataManager dataManager_;
};

} // namespace DrumMachine

#endif // PATTERN_MANAGER_H
