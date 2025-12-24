#include "DataManager.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

namespace DrumMachine {

DataManager::DataManager()
{
}

bool DataManager::savePattern(const std::string& filePath, const Pattern& pattern)
{
    try {
        std::string jsonStr = patternToJson(pattern);
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        file << jsonStr;
        file.close();
        std::cout << "Pattern saved to: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving pattern: " << e.what() << std::endl;
        return false;
    }
}

bool DataManager::loadPattern(const std::string& filePath, Pattern& pattern)
{
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filePath << std::endl;
            return false;
        }
        std::string jsonStr((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        bool result = patternFromJson(jsonStr, pattern);
        if (result) {
            std::cout << "Pattern loaded from: " << filePath << std::endl;
        }
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Error loading pattern: " << e.what() << std::endl;
        return false;
    }
}

std::string DataManager::patternToJson(const Pattern& pattern) const
{
    json j;
    
    // Serialize all tracks
    for (uint32_t track = 0; track < Pattern::NUM_TRACKS; ++track) {
        const auto& trackObj = pattern.getTrack(track);
        json trackJson;
        
        trackJson["name"] = trackObj.name;
        trackJson["samplePath"] = trackObj.samplePath;
        trackJson["volume"] = trackObj.volume;
        trackJson["muted"] = trackObj.muted;
        
        // Serialize steps
        std::vector<uint8_t> steps(trackObj.steps.begin(), trackObj.steps.end());
        trackJson["steps"] = steps;
        
        j["tracks"][track] = trackJson;
    }
    
    return j.dump(2);
}

bool DataManager::patternFromJson(const std::string& jsonString, Pattern& pattern)
{
    try {
        json j = json::parse(jsonString);
        
        // Deserialize tracks
        if (j.contains("tracks")) {
            auto& tracksJson = j["tracks"];
            for (uint32_t track = 0; track < Pattern::NUM_TRACKS && track < tracksJson.size(); ++track) {
                auto& trackJson = tracksJson[track];
                
                // Load step data
                if (trackJson.contains("steps")) {
                    auto stepsArray = trackJson["steps"].get<std::vector<uint8_t>>();
                    for (uint32_t step = 0; step < stepsArray.size() && step < Pattern::STEPS_PER_BAR; ++step) {
                        pattern.setStepActive(track, step, stepsArray[step] != 0);
                    }
                }
                
                // Load track properties
                if (trackJson.contains("muted")) {
                    pattern.setTrackMuted(track, trackJson["muted"].get<bool>());
                }
                if (trackJson.contains("volume")) {
                    pattern.setTrackVolume(track, trackJson["volume"].get<float>());
                }
                if (trackJson.contains("samplePath")) {
                    pattern.setTrackSample(track, trackJson["samplePath"].get<std::string>());
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing pattern JSON: " << e.what() << std::endl;
        return false;
    }
}

} // namespace DrumMachine
