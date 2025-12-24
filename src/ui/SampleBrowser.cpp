#include "SampleBrowser.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Pattern.h"
#include "../audio/SamplePlayer.h"
#include "../core/ParameterBus.h"
#include <imgui.h>
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

namespace DrumMachine {

SampleBrowser::SampleBrowser()
    : showBrowser_(false), selectedTrackForLoading_(0)
{
    currentDirectory_ = fs::current_path().string();
    std::memset(sampleFilterBuffer_, 0, sizeof(sampleFilterBuffer_));
}

std::vector<std::string> SampleBrowser::getSampleFiles(const std::string& directory)
{
    std::vector<std::string> samples;

    try {
        fs::path dirPath(directory);
        if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
            return samples;
        }

        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                // Convert to lowercase for comparison
                std::transform(ext.begin(), ext.end(), ext.begin(), 
                    [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
                
                if (ext == ".wav" || ext == ".mp3" || ext == ".flac" || ext == ".aiff") {
                    samples.push_back(entry.path().filename().string());
                }
            }
        }

        // Sort alphabetically
        std::sort(samples.begin(), samples.end());
    } catch (const std::exception& e) {
        std::cerr << "Error browsing directory: " << e.what() << std::endl;
    }

    return samples;
}

bool SampleBrowser::loadSampleForTrack(const std::string& filePath, uint32_t trackIndex, Sequencer* sequencer, SamplePlayer* samplePlayer)
{
    if (!sequencer || !samplePlayer || trackIndex >= 8) {
        return false;
    }

    try {
        // Load sample into SamplePlayer
        if (!samplePlayer->loadSample(filePath)) {
            std::cerr << "Failed to load sample: " << filePath << std::endl;
            return false;
        }

        // Update Pattern to reference this sample
        sequencer->getPattern().setTrackSample(trackIndex, filePath);

        // Publish parameter change
        ParameterChange change;
        change.type = ParameterType::TRACK_SAMPLE;
        change.value = filePath;
        change.trackIndex = trackIndex;
        change.moduleId = "sample_browser";
        ParameterBus::getInstance().publish(change);

        std::cout << "Loaded sample for track " << trackIndex << ": " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading sample: " << e.what() << std::endl;
        return false;
    }
}

void SampleBrowser::render(Sequencer* sequencer, SamplePlayer* samplePlayer, uint32_t selectedTrack)
{
    if (!ImGui::Begin("Sample Browser", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Load samples for tracks");
    ImGui::Separator();

    // Track selector
    ImGui::Text("Selected Track: %u", selectedTrack);
    ImGui::SameLine();
    if (ImGui::Button("Browse Samples...")) {
        showBrowser_ = true;
        selectedTrackForLoading_ = selectedTrack;
    }

    ImGui::Spacing();

    // Display current track samples if available
    if (sequencer) {
        ImGui::Text("Track Sample Paths:");
        ImGui::Separator();

        for (uint32_t i = 0; i < 8; ++i) {
            // Get sample path from pattern (would need to add getter to Pattern class)
            ImGui::Text("Track %u: [sample path]", i);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();

    // File browser modal
    if (showBrowser_) {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Browse Samples", &showBrowser_, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Current Directory: %s", currentDirectory_.c_str());
            ImGui::Separator();

            // Directory navigation
            if (ImGui::Button("Parent Directory") && currentDirectory_ != "/") {
                fs::path currentPath(currentDirectory_);
                currentDirectory_ = currentPath.parent_path().string();
            }

            ImGui::Spacing();
            ImGui::Text("Available Samples:");
            ImGui::Separator();

            std::vector<std::string> samples = getSampleFiles(currentDirectory_);

            for (const auto& sample : samples) {
                if (ImGui::Selectable(sample.c_str())) {
                    fs::path fullPath = fs::path(currentDirectory_) / sample;
                    if (loadSampleForTrack(fullPath.string(), selectedTrackForLoading_, sequencer, samplePlayer)) {
                        ImGui::OpenPopup("Load Success");
                        showBrowser_ = false;
                    } else {
                        ImGui::OpenPopup("Load Error");
                    }
                }
            }

            if (samples.empty()) {
                ImGui::Text("No audio files found (.wav, .mp3, .flac, .aiff)");
            }

            ImGui::Spacing();
            ImGui::Separator();

            if (ImGui::Button("Close", ImVec2(150, 0))) {
                showBrowser_ = false;
            }

            ImGui::EndPopup();
        }
    }

    // Success/Error dialogs
    if (ImGui::BeginPopupModal("Load Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Sample loaded successfully for track %u!", selectedTrackForLoading_);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Load Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to load sample. Check file format and path.");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace DrumMachine
