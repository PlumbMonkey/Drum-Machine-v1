#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <cstdint>
#include <memory>
#include <vector>

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

    // Set sequencer reference (for playback callback)
    void setSequencer(Sequencer* sequencer);

    // Audio callback (called by RtAudio on audio thread)
    // Static wrapper for C callback
    static int audioCallback(void* outputBuffer, void* inputBuffer,
                              unsigned int nFrames, double streamTime,
                              RtAudioStreamStatus status, void* userData);

private:
    uint32_t sampleRate_;
    bool isRunning_;
    Sequencer* sequencer_;
    
    // RtAudio instance (opaque pointer to avoid exposing RtAudio.h)
    void* rtAudioHandle_;

    // Internal callback implementation
    int processAudio(void* outputBuffer, unsigned int nFrames);
};

} // namespace DrumMachine

#endif // AUDIO_ENGINE_H
