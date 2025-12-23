#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "../sequencer/Pattern.h"
#include <string>

namespace DrumMachine {

/**
 * DataManager
 * 
 * Serialization and deserialization of project data.
 * JSON format using nlohmann/json.
 * Milestone 6: Save/load functionality
 */
class DataManager {
public:
    DataManager();

    // Save pattern to JSON file
    bool savePattern(const std::string& filePath, const Pattern& pattern);

    // Load pattern from JSON file
    bool loadPattern(const std::string& filePath, Pattern& pattern);

    // Export pattern to JSON string
    std::string patternToJson(const Pattern& pattern) const;

    // Import pattern from JSON string
    bool patternFromJson(const std::string& jsonString, Pattern& pattern);

private:
    // Helper: Convert time signature string to active steps count
    uint32_t getActiveStepsForTimeSignature(const std::string& timeSig) const;
};

} // namespace DrumMachine

#endif // DATA_MANAGER_H
