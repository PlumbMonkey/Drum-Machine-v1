#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "Pattern.h"
#include "Transport.h"
#include <memory>
#include <vector>

namespace DrumMachine {

/**
 * Sequencer
 * 
 * Main sequencer logic.
 * Manages pattern playback, step scheduling, and timing.
 * Milestone 2: Step sequencer logic with swing
 */
class Sequencer {
public:
    Sequencer(uint32_t sampleRate = 44100);

    // Pattern management
    Pattern& getPattern() { return pattern_; }
    const Pattern& getPattern() const { return pattern_; }

    // Transport access
    Transport& getTransport() { return transport_; }
    const Transport& getTransport() const { return transport_; }

    // Check if a note should trigger on this step
    // Takes swing into account
    bool shouldTrigger(uint32_t trackIndex, uint32_t step, uint64_t currentSample);

    // Advance sequencer by one audio frame
    void advanceFrame(uint32_t numFrames);

    // Get swing-delayed sample position for a step
    uint32_t getSwingDelayedSample(uint32_t stepIndex, uint32_t baseSample) const;

private:
    Pattern pattern_;
    Transport transport_;
    uint32_t sampleRate_;
    uint64_t absoluteFrameCounter_; // Global frame counter for timing
};

} // namespace DrumMachine

#endif // SEQUENCER_H
