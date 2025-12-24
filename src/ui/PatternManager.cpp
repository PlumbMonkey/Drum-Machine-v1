#include "PatternManager.h"
#include "../sequencer/Pattern.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace DrumMachine {

PatternManager::PatternManager()
{
    // Ensure patterns and MIDI directories exist
    try {
        fs::path patternDir(getPatternDirectory());
        if (!fs::exists(patternDir)) {
            fs::create_directories(patternDir);
        }
        
        fs::path midiDir(getMidiDirectory());
        if (!fs::exists(midiDir)) {
            fs::create_directories(midiDir);
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to create directories: " << e.what() << std::endl;
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

bool PatternManager::exportToMidi(const std::string& filename, const Pattern& pattern,
                                  float tempo)
{
    std::string filePath = std::string(getMidiDirectory()) + filename;
    // Check if filename ends with .mid (C++17 compatible)
    const std::string midExt = ".mid";
    if (filePath.length() < midExt.length() || 
        filePath.substr(filePath.length() - midExt.length()) != midExt) {
        filePath += ".mid";
    }
    return midiManager_.exportToMidi(filePath, pattern, tempo);
}

bool PatternManager::importFromMidi(const std::string& filename, Pattern& pattern)
{
    std::string filePath = std::string(getMidiDirectory()) + filename;
    // Check if filename ends with .mid (C++17 compatible)
    const std::string midExt = ".mid";
    if (filePath.length() < midExt.length() || 
        filePath.substr(filePath.length() - midExt.length()) != midExt) {
        filePath += ".mid";
    }
    return midiManager_.importFromMidi(filePath, pattern);
}

std::vector<std::string> PatternManager::getAvailableMidiFiles() const
{
    std::vector<std::string> midiFiles;
    
    try {
        fs::path midiDir(getMidiDirectory());
        if (!fs::exists(midiDir)) {
            return midiFiles;
        }

        for (const auto& entry : fs::directory_iterator(midiDir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                // Convert to lowercase for comparison (C++17 compatible)
                std::transform(ext.begin(), ext.end(), ext.begin(),
                               [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
                
                if (ext == ".mid" || ext == ".midi") {
                    std::string filename = entry.path().filename().string();
                    // Remove extension
                    size_t dotPos = filename.find_last_of('.');
                    if (dotPos != std::string::npos) {
                        filename = filename.substr(0, dotPos);
                    }
                    midiFiles.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error listing MIDI files: " << e.what() << std::endl;
    }

    return midiFiles;
}

} // namespace DrumMachine
