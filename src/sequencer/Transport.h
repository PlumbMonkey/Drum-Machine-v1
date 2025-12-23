#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <cstdint>
#include <string>

namespace DrumMachine {

/**
 * Transport
 * 
 * Global timing and transport state.
 * Controls tempo, time signature, and playback state.
 * Lock-free for real-time thread safety.
 */
class Transport {
public:
    enum class PlayState {
        Stopped,
        Playing
    };

    Transport();

    // Playback control
    void play();
    void stop();
    void reset();

    // Tempo (60-180 BPM)
    void setTempo(float bpm);
    float getTempo() const { return tempoInBPM_; }

    // Swing (0.0 to 0.6)
    void setSwing(float swing);
    float getSwing() const { return swing_; }

    // Time signature
    void setTimeSignature(const std::string& timeSig);
    std::string getTimeSignature() const { return timeSignature_; }

    // Bar count (1-5 bars per pattern)
    void setBarCount(uint32_t bars);
    uint32_t getBarCount() const { return barCount_; }

    // Playback state
    PlayState getPlayState() const { return playState_; }

    // Current step in grid (0-15 for single bar, extends for multi-bar)
    uint32_t getCurrentStep() const { return currentStep_; }

    // Current bar (0-based)
    uint32_t getCurrentBar() const { return currentBar_; }

    // Samples per step (at current sample rate and tempo)
    uint32_t getSamplesPerStep(uint32_t sampleRate) const;

    // Get number of active steps for current time signature
    uint32_t getActiveStepsPerBar() const;

    // Advance playback by one audio frame
    void advanceFrame(uint32_t sampleRate);

private:
    PlayState playState_;
    float tempoInBPM_;
    float swing_;           // 0.0 to 0.6
    std::string timeSignature_; // "4/4", "3/4", "6/8"
    uint32_t barCount_;
    uint32_t currentStep_;  // 0-15 within current bar
    uint32_t currentBar_;   // 0 to (barCount_ - 1)
    uint64_t frameCounter_; // For sub-sample timing
};

} // namespace DrumMachine

#endif // TRANSPORT_H
