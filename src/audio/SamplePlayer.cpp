#include "SamplePlayer.h"
#include <iostream>
#include <cstring>

namespace DrumMachine {

SamplePlayer::SamplePlayer(uint32_t engineSampleRate)
    : engineSampleRate_(engineSampleRate), playbackPosition_(0), 
      isPlaying_(false), originalSampleRate_(44100)
{
    // TODO: Initialize dr_wav
}

SamplePlayer::~SamplePlayer()
{
    // TODO: Cleanup dr_wav
}

bool SamplePlayer::loadSample(const std::string& filePath)
{
    // TODO: Use dr_wav to load WAV file
    // - Read file metadata
    // - Read audio data
    // - Store in sampleData_
    // - Call resample() if sample rate differs
    
    std::cout << "Loaded sample: " << filePath << std::endl;
    return true;
}

void SamplePlayer::resample()
{
    // TODO: Implement resampling if originalSampleRate_ != engineSampleRate_
    // Use simple linear interpolation for now
}

void SamplePlayer::start()
{
    playbackPosition_ = 0;
    isPlaying_ = true;
}

void SamplePlayer::stop()
{
    isPlaying_ = false;
}

void SamplePlayer::reset()
{
    playbackPosition_ = 0;
}

uint32_t SamplePlayer::readFrames(float* outputBuffer, uint32_t numFrames)
{
    if (!isPlaying_ || sampleData_.empty()) {
        std::memset(outputBuffer, 0, numFrames * sizeof(float));
        return 0;
    }

    // TODO: Implement frame reading with loop and stop behavior
    uint32_t framesRead = 0;
    
    for (uint32_t i = 0; i < numFrames; ++i) {
        if (playbackPosition_ < sampleData_.size()) {
            outputBuffer[i] = sampleData_[playbackPosition_++];
            framesRead++;
        } else {
            // End of sample - retrigger or stop?
            outputBuffer[i] = 0.0f;
            isPlaying_ = false;
            break;
        }
    }

    return framesRead;
}

} // namespace DrumMachine
