#ifndef SAMPLE_PLAYER_H
#define SAMPLE_PLAYER_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace DrumMachine {

/**
 * SamplePlayer
 * 
 * Loads and plays WAV samples using dr_wav.
 * Handles resampling to engine sample rate.
 * Per-track playback with retrigger behavior.
 */
class SamplePlayer {
public:
    SamplePlayer(uint32_t engineSampleRate);
    ~SamplePlayer();

    // Load a WAV file
    bool loadSample(const std::string& filePath);

    // Get sample data
    const std::vector<float>& getSampleData() const { return sampleData_; }

    // Playback position (in samples)
    uint32_t getPlaybackPosition() const { return playbackPosition_; }
    
    // Is sample currently playing?
    bool isPlaying() const { return isPlaying_; }

    // Start playback
    void start();

    // Stop playback
    void stop();

    // Reset playback to beginning
    void reset();

    // Get next audio frame(s)
    // Returns number of samples read into buffer
    uint32_t readFrames(float* outputBuffer, uint32_t numFrames);

private:
    uint32_t engineSampleRate_;
    std::vector<float> sampleData_;
    uint32_t playbackPosition_;
    bool isPlaying_;
    uint32_t originalSampleRate_;

    // Helper: Resample sample data to engine sample rate
    void resample();
};

} // namespace DrumMachine

#endif // SAMPLE_PLAYER_H
