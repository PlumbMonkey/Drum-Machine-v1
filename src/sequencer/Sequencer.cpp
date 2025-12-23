#include "Sequencer.h"
#include <cmath>

namespace DrumMachine {

Sequencer::Sequencer(uint32_t sampleRate)
    : sampleRate_(sampleRate), absoluteFrameCounter_(0)
{
}

bool Sequencer::shouldTrigger(uint32_t trackIndex, uint32_t step, uint64_t currentSample)
{
    // Check if step is active for this track
    if (!pattern_.isStepActive(trackIndex, step)) {
        return false;
    }

    // Check if track is muted
    if (pattern_.isTrackMuted(trackIndex)) {
        return false;
    }

    // TODO: Implement timing check with swing
    // Get expected sample for this step (considering swing)
    // Return true if currentSample matches

    return true;
}

void Sequencer::advanceFrame(uint32_t numFrames)
{
    transport_.advanceFrame(sampleRate_);
    absoluteFrameCounter_ += numFrames;
}

uint32_t Sequencer::getSwingDelayedSample(uint32_t stepIndex, uint32_t baseSample) const
{
    // Swing applies to even-numbered steps (2, 4, 6, etc.)
    if (stepIndex % 2 == 0) {
        float swingAmount = transport_.getSwing();
        uint32_t samplesPerStep = transport_.getSamplesPerStep(sampleRate_);
        uint32_t swingDelay = static_cast<uint32_t>(swingAmount * samplesPerStep);
        return baseSample + swingDelay;
    }

    return baseSample;
}

} // namespace DrumMachine
