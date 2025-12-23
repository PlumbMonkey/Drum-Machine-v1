#include "AudioEngine.h"
#include "../sequencer/Sequencer.h"
#include <RtAudio.h>
#include <iostream>
#include <cstring>

using namespace rt::audio;

namespace DrumMachine {

// RtAudio wrapper to avoid exposing RtAudio.h in header
class AudioEngine::RtAudioWrapper {
public:
    rt::audio::RtAudio rtAudio;
};

AudioEngine::AudioEngine(uint32_t sampleRate)
    : sampleRate_(sampleRate), isRunning_(false), sequencer_(nullptr), 
      totalFramesProcessed_(0)
{
    rtAudio_ = std::make_unique<RtAudioWrapper>();
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

bool AudioEngine::initialize()
{
    if (isRunning_) {
        std::cerr << "Audio engine already running" << std::endl;
        return false;
    }

    // Get available devices
    std::vector<unsigned int> devices = rtAudio_->rtAudio.getDeviceIds();
    if (devices.empty()) {
        std::cerr << "No audio devices found" << std::endl;
        return false;
    }

    // Use default output device
    unsigned int deviceId = rtAudio_->rtAudio.getDefaultOutputDevice();
    RtAudio::DeviceInfo deviceInfo = rtAudio_->rtAudio.getDeviceInfo(deviceId);

    std::cout << "Using audio device: " << deviceInfo.name << std::endl;
    std::cout << "Channels: " << deviceInfo.outputChannels << std::endl;
    std::cout << "Sample rate: " << sampleRate_ << " Hz" << std::endl;

    // Setup audio parameters
    rt::audio::RtAudio::StreamParameters parameters;
    parameters.deviceId = deviceId;
    parameters.nChannels = 2; // Stereo output
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256; // 256 samples per callback

    // Open stream
    rt::audio::RtAudioCallback callback = 
        [this](void* outputBuffer, void* inputBuffer,
               unsigned int nFrames, double streamTime,
               rt::audio::RtAudioStreamStatus status,
               void* userData) -> int {
            return this->processAudio(outputBuffer, nFrames);
        };
    
    rtAudio_->rtAudio.openStream(&parameters, nullptr, rt::audio::RTAUDIO_FLOAT32,
                                  sampleRate_, &bufferFrames, 
                                  callback, 
                                  static_cast<void*>(this), nullptr);

    // Start stream
    rtAudio_->rtAudio.startStream();

    isRunning_ = true;
    std::cout << "Audio engine initialized successfully" << std::endl;
    return true;
}

void AudioEngine::shutdown()
{
    if (!isRunning_) {
        return;
    }

    if (rtAudio_->rtAudio.isStreamOpen()) {
        rtAudio_->rtAudio.stopStream();
        rtAudio_->rtAudio.closeStream();
    }
    isRunning_ = false;
    std::cout << "Audio engine shutdown" << std::endl;
}

bool AudioEngine::isRunning() const
{
    return isRunning_;
}

void AudioEngine::setSequencer(Sequencer* sequencer)
{
    sequencer_ = sequencer;
}

int AudioEngine::processAudio(void* outputBuffer, unsigned int nFrames)
{
    float* buffer = static_cast<float*>(outputBuffer);
    
    // Zero out buffer first
    std::memset(buffer, 0, nFrames * 2 * sizeof(float)); // 2 channels

    // Get audio from sequencer (if available)
    if (sequencer_) {
        // Read samples from sequencer into buffer
        // This will be filled by sequencer's playback logic
        // For now, silence
    }

    // Update frame counter (atomic, lock-free)
    totalFramesProcessed_.fetch_add(nFrames, std::memory_order_release);

    return 0; // Success
}

} // namespace DrumMachine
