#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <cstdint>
#include <memory>
#include <vector>
#include <atomic>

namespace DrumMachine {

class Sequencer; // Forward declaration

/**
 * AudioEngine
 * 
 * Manages real-time audio I/O using RtAudio.
 * Runs audio callback on separate thread.
 * Lock-free communication with main UI thread.
 * 
 * Milestone 1: Play one sample on a loop at 120 BPM
 */
class AudioEngine {
public:
    AudioEngine(uint32_t sampleRate = 44100);
    ~AudioEngine();

    // Initialize audio device and start callback
    bool initialize();
    
    // Shutdown audio device
    void shutdown();

    // Is audio running?
    bool isRunning() const;

    // Get sample rate
    uint32_t getSampleRate() const { return sampleRate_; }

    // Get total frames processed (for timing verification)
    uint64_t getTotalFramesProcessed() const { return totalFramesProcessed_.load(); }

    // Set sequencer reference (for playback callback)
    void setSequencer(Sequencer* sequencer);

private:
    uint32_t sampleRate_;
    bool isRunning_;
    Sequencer* sequencer_;
    std::atomic<uint64_t> totalFramesProcessed_;
    
    // RtAudio instance (forward declared, defined in .cpp)
    class RtAudioWrapper;
    std::unique_ptr<RtAudioWrapper> rtAudio_;

    // Internal callback implementation
    int processAudio(void* outputBuffer, unsigned int nFrames);
};

} // namespace DrumMachine

#endif // AUDIO_ENGINE_H
