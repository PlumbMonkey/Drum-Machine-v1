#include "Transport.h"
#include <cmath>
#include <algorithm>

namespace DrumMachine {

Transport::Transport()
    : playState_(PlayState::Stopped), tempoInBPM_(120.0f), swing_(0.0f),
      timeSignature_("4/4"), barCount_(1), currentStep_(0), currentBar_(0),
      frameCounter_(0)
{
}

void Transport::play()
{
    playState_ = PlayState::Playing;
}

void Transport::stop()
{
    playState_ = PlayState::Stopped;
}

void Transport::reset()
{
    currentStep_ = 0;
    currentBar_ = 0;
    frameCounter_ = 0;
}

void Transport::setTempo(float bpm)
{
    tempoInBPM_ = std::clamp(bpm, 60.0f, 180.0f);
}

void Transport::setSwing(float swing)
{
    swing_ = std::clamp(swing, 0.0f, 0.6f);
}

void Transport::setTimeSignature(const std::string& timeSig)
{
    timeSignature_ = timeSig;
}

void Transport::setBarCount(uint32_t bars)
{
    barCount_ = std::clamp(bars, 1u, 5u);
}

uint32_t Transport::getSamplesPerStep(uint32_t sampleRate) const
{
    // 16 steps per bar, 4 beats per bar (4/4)
    // BPM = beats per minute
    // beats per second = BPM / 60
    // seconds per beat = 60 / BPM
    // seconds per step = (60 / BPM) / 4
    // samples per step = (60 / BPM / 4) * sampleRate

    float secondsPerStep = (60.0f / tempoInBPM_) / 4.0f;
    return static_cast<uint32_t>(secondsPerStep * sampleRate);
}

uint32_t Transport::getActiveStepsPerBar() const
{
    if (timeSignature_ == "3/4") {
        return 12; // 3 beats * 4 steps per beat
    } else if (timeSignature_ == "6/8") {
        return 12; // 6 beats * 2 steps per beat (or 6 steps if thinking differently)
    }
    return 16; // 4/4: 4 beats * 4 steps per beat
}

void Transport::advanceFrame(uint32_t sampleRate)
{
    if (playState_ != PlayState::Playing) {
        return;
    }

    uint32_t samplesPerStep = getSamplesPerStep(sampleRate);
    frameCounter_++;

    if (frameCounter_ >= samplesPerStep) {
        frameCounter_ = 0;
        currentStep_++;

        if (currentStep_ >= 16) {
            currentStep_ = 0;
            currentBar_++;

            if (currentBar_ >= barCount_) {
                currentBar_ = 0;
            }
        }
    }
}

} // namespace DrumMachine
