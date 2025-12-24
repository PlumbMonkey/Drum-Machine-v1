#ifndef SAMPLE_PLAYER_H
#define SAMPLE_PLAYER_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <atomic>

namespace DrumMachine {

/**
 * SamplePlayer
 * 
 * Loads and plays WAV samples using dr_wav.
 * Handles resampling to engine sample rate.
 * Per-track playback with retrigger behavior.
 * Milestone 1: Basic sample loading and playback
 */
class SamplePlayer {
public:
    SamplePlayer(uint32_t engineSampleRate);
    ~SamplePlayer();

    // Load a WAV file
    bool loadSample(const std::string& filePath);

    // Get sample data
    const std::vector<float>& getSampleData() const { return sampleData_; }

    // Get duration in seconds
    float getDurationSeconds() const;

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

    // Trigger sample (reset and start)
    void trigger() { reset(); start(); }

    // Get next audio frames
    // Reads interleaved stereo samples (L/R/L/R...)
    // Returns number of frames actually read
    uint32_t readFrames(float* outputBuffer, uint32_t numFrames, bool loop = true);

    // Get sample rate of loaded sample
    uint32_t getOriginalSampleRate() const { return originalSampleRate_; }

    // Get number of channels in loaded sample
    uint32_t getChannelCount() const { return channelCount_; }

private:
    uint32_t engineSampleRate_;
    std::vector<float> sampleData_;       // Interleaved audio data
    std::atomic<uint32_t> playbackPosition_;  // Current position in sample (atomic for thread safety)
    std::atomic<bool> isPlaying_;             // Playing state (atomic for thread safety)
    std::atomic<bool> pendingTrigger_;        // Flag set by UI thread, consumed by audio thread
    uint32_t originalSampleRate_;
    uint32_t channelCount_;               // 1 = mono, 2 = stereo
    uint32_t totalFrames_;                // Total frames in sample

    // Helper: Resample sample data to engine sample rate using linear interpolation
    void resample(const std::vector<float>& input, uint32_t inputChannels,
                  uint32_t inputSampleRate);
};

} // namespace DrumMachine

#endif // SAMPLE_PLAYER_H
