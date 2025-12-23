#include "SamplePlayer.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>

// dr_wav implementation - single header
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace DrumMachine {

SamplePlayer::SamplePlayer(uint32_t engineSampleRate)
    : engineSampleRate_(engineSampleRate), playbackPosition_(0), 
      isPlaying_(false), originalSampleRate_(44100), channelCount_(1), totalFrames_(0)
{
}

SamplePlayer::~SamplePlayer()
{
    // Vector cleanup is automatic
}

bool SamplePlayer::loadSample(const std::string& filePath)
{
    unsigned int channels = 0;
    unsigned int sampleRate = 0;
    drwav_uint64 frameCount = 0;

    // Load WAV file
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(
        filePath.c_str(), &channels, &sampleRate, &frameCount, nullptr);

    if (!pSampleData) {
        std::cerr << "Failed to load WAV file: " << filePath << std::endl;
        return false;
    }

    std::cout << "Loaded sample: " << filePath << std::endl;
    std::cout << "  Channels: " << channels << std::endl;
    std::cout << "  Sample Rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "  Frames: " << frameCount << " (" << (frameCount / static_cast<float>(sampleRate)) << " seconds)" << std::endl;

    originalSampleRate_ = sampleRate;
    channelCount_ = channels;
    totalFrames_ = frameCount;

    // Copy to vector (interleaved format)
    std::vector<float> rawData(pSampleData, pSampleData + frameCount * channels);
    
    // Free dr_wav allocation
    drwav_free(pSampleData, nullptr);

    // Resample if needed
    if (sampleRate != engineSampleRate_) {
        std::cout << "Resampling from " << sampleRate << " Hz to " << engineSampleRate_ << " Hz" << std::endl;
        resample(rawData, channels, sampleRate);
    } else {
        sampleData_ = rawData;
    }

    reset();
    return true;
}

void SamplePlayer::resample(const std::vector<float>& input, uint32_t inputChannels,
                            uint32_t inputSampleRate)
{
    if (input.empty()) {
        return;
    }

    // Calculate output size
    uint32_t inputFrames = input.size() / inputChannels;
    float ratio = static_cast<float>(engineSampleRate_) / inputSampleRate;
    uint32_t outputFrames = static_cast<uint32_t>(inputFrames * ratio);

    sampleData_.clear();
    sampleData_.reserve(outputFrames * inputChannels);

    // Linear interpolation resampling
    for (uint32_t outFrame = 0; outFrame < outputFrames; ++outFrame) {
        float inFrame = outFrame / ratio;
        uint32_t inFrameLow = static_cast<uint32_t>(inFrame);
        uint32_t inFrameHigh = std::min(inFrameLow + 1, inputFrames - 1);
        float fraction = inFrame - inFrameLow;

        // Interpolate each channel
        for (uint32_t ch = 0; ch < inputChannels; ++ch) {
            float lowSample = input[inFrameLow * inputChannels + ch];
            float highSample = input[inFrameHigh * inputChannels + ch];
            float interpolated = lowSample + (highSample - lowSample) * fraction;
            sampleData_.push_back(interpolated);
        }
    }

    totalFrames_ = outputFrames;
    std::cout << "Resampled to " << outputFrames << " frames" << std::endl;
}

float SamplePlayer::getDurationSeconds() const
{
    if (engineSampleRate_ == 0) return 0.0f;
    return totalFrames_ / static_cast<float>(engineSampleRate_);
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

uint32_t SamplePlayer::readFrames(float* outputBuffer, uint32_t numFrames, bool loop)
{
    if (!isPlaying_ || sampleData_.empty()) {
        std::memset(outputBuffer, 0, numFrames * channelCount_ * sizeof(float));
        return 0;
    }

    uint32_t framesRead = 0;
    
    for (uint32_t i = 0; i < numFrames; ++i) {
        uint32_t sampleIndex = playbackPosition_ * channelCount_;

        if (sampleIndex + channelCount_ - 1 < sampleData_.size()) {
            // Copy interleaved channels
            for (uint32_t ch = 0; ch < channelCount_; ++ch) {
                outputBuffer[i * channelCount_ + ch] = sampleData_[sampleIndex + ch];
            }
            framesRead++;
        } else {
            // End of sample reached
            if (loop) {
                playbackPosition_ = 0;
                sampleIndex = 0;
                for (uint32_t ch = 0; ch < channelCount_; ++ch) {
                    outputBuffer[i * channelCount_ + ch] = sampleData_[sampleIndex + ch];
                }
                framesRead++;
            } else {
                // Fill rest with silence and stop
                for (uint32_t ch = 0; ch < channelCount_; ++ch) {
                    outputBuffer[i * channelCount_ + ch] = 0.0f;
                }
                isPlaying_ = false;
                break;
            }
        }

        playbackPosition_++;
    }

    return framesRead;
}

} // namespace DrumMachine
