#ifndef SAMPLE_BROWSER_H
#define SAMPLE_BROWSER_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace DrumMachine {

class Sequencer;
class SamplePlayer;

/**
 * SampleBrowser
 * 
 * File browser UI for loading WAV samples.
 * Allows per-track sample assignment with visual feedback.
 * Integrates with SamplePlayer and Pattern system.
 * 
 * Milestone 4: Sample Management UI
 */
class SampleBrowser {
public:
    SampleBrowser();

    // Render the sample browser UI
    void render(Sequencer* sequencer, SamplePlayer* samplePlayer, uint32_t selectedTrack);

private:
    // Current directory being browsed
    std::string currentDirectory_;
    char sampleFilterBuffer_[256];

    // UI State
    bool showBrowser_;
    uint32_t selectedTrackForLoading_;

    // Helper methods
    std::vector<std::string> getSampleFiles(const std::string& directory);
    bool loadSampleForTrack(const std::string& filePath, uint32_t trackIndex, Sequencer* sequencer, SamplePlayer* samplePlayer);
    void renderDirectoryTree();
    void renderSampleList(const std::vector<std::string>& samples);
};

} // namespace DrumMachine

#endif // SAMPLE_BROWSER_H
