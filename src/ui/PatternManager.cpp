#include "PatternManager.h"
#include "../sequencer/Pattern.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace DrumMachine {

PatternManager::PatternManager()
{
    // Ensure patterns directory exists
    fs::path patternDir(getPatternDirectory());
    if (!fs::exists(patternDir)) {
        try {
            fs::create_directories(patternDir);
        } catch (const std::exception& e) {
            std::cerr << "Failed to create pattern directory: " << e.what() << std::endl;
        }
    }
}

bool PatternManager::savePattern(const std::string& filename, const Pattern& pattern)
{
    std::string filePath = std::string(getPatternDirectory()) + filename;
    // Check if filename ends with .json (C++17 compatible)
    const std::string jsonExt = ".json";
    if (filePath.length() < jsonExt.length() || 
        filePath.substr(filePath.length() - jsonExt.length()) != jsonExt) {
        filePath += ".json";
    }
    return dataManager_.savePattern(filePath, pattern);
}

bool PatternManager::loadPattern(const std::string& filename, Pattern& pattern)
{
    std::string filePath = std::string(getPatternDirectory()) + filename;
    // Check if filename ends with .json (C++17 compatible)
    const std::string jsonExt = ".json";
    if (filePath.length() < jsonExt.length() || 
        filePath.substr(filePath.length() - jsonExt.length()) != jsonExt) {
        filePath += ".json";
    }
    return dataManager_.loadPattern(filePath, pattern);
}

std::vector<std::string> PatternManager::getAvailablePatterns() const
{
    std::vector<std::string> patterns;
    
    try {
        fs::path patternDir(getPatternDirectory());
        if (!fs::exists(patternDir)) {
            return patterns;
        }

        for (const auto& entry : fs::directory_iterator(patternDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().filename().string();
                // Remove .json extension (C++17 compatible)
                const std::string jsonExt = ".json";
                if (filename.length() >= jsonExt.length() && 
                    filename.substr(filename.length() - jsonExt.length()) == jsonExt) {
                    filename = filename.substr(0, filename.length() - jsonExt.length());
                }
                patterns.push_back(filename);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error listing patterns: " << e.what() << std::endl;
    }

    return patterns;
}

} // namespace DrumMachine
