#include "DrumMachine.h"
#include "audio/AudioEngine.h"
#include "audio/SamplePlayer.h"
#include "audio/MidiManager.h"
#include "sequencer/Sequencer.h"
#include "sequencer/Transport.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace DrumMachine;

/**
 * CLI Version - No UI
 * Tests core audio/sequencer functionality without SDL2/OpenGL
 */
int main(int argc, char* argv[])
{
    std::cout << "======================================" << std::endl;
    std::cout << "Drum Machine v" << DRUM_MACHINE_VERSION << std::endl;
    std::cout << "Milestone 5: MIDI Foundation (CLI)" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;

    // Configuration
    uint32_t sampleRate = 44100;
    std::string samplePath = "assets/samples/test_kick.wav";

    // Initialize audio engine
    std::cout << "[1/4] Initializing audio engine..." << std::endl;
    AudioEngine audioEngine(sampleRate);
    if (!audioEngine.initialize()) {
        std::cerr << "FAILED to initialize audio engine" << std::endl;
        return 1;
    }
    std::cout << "      Audio engine OK" << std::endl;
    std::cout << std::endl;

    // Initialize sequencer
    std::cout << "[2/4] Initializing sequencer..." << std::endl;
    Sequencer sequencer(sampleRate);
    audioEngine.setSequencer(&sequencer);
    sequencer.getTransport().setTempo(120.0f);
    sequencer.getTransport().setTimeSignature("4/4");
    sequencer.getTransport().setBarCount(1);
    std::cout << "      Sequencer OK" << std::endl;
    std::cout << std::endl;

    // Load sample
    std::cout << "[3/4] Loading sample..." << std::endl;
    SamplePlayer samplePlayer(sampleRate);
    if (!samplePlayer.loadSample(samplePath)) {
        std::cerr << "FAILED to load sample: " << samplePath << std::endl;
        audioEngine.shutdown();
        return 1;
    }
    std::cout << "      Sample OK (" << samplePlayer.getDurationSeconds() << " seconds)" << std::endl;
    samplePlayer.start();
    std::cout << std::endl;

    // Initialize MIDI manager
    std::cout << "[4/4] Initializing MIDI..." << std::endl;
    MidiManager midiManager;
    if (!midiManager.initialize()) {
        std::cout << "      WARNING: MIDI initialization failed (continuing without MIDI)" << std::endl;
    } else {
        std::cout << "      MIDI OK" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Audio engine running for 30 seconds..." << std::endl;
    std::cout << "Press Ctrl+C to exit early" << std::endl;
    std::cout << std::endl;

    // Run for 30 seconds
    auto start = std::chrono::steady_clock::now();
    auto duration = std::chrono::seconds(30);
    
    while (std::chrono::steady_clock::now() - start < duration) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        uint64_t frames = audioEngine.getTotalFramesProcessed();
        double seconds = static_cast<double>(frames) / sampleRate;
        std::cout << "\rRunning: " << seconds << " seconds (" << frames << " frames)" << std::flush;
    }

    std::cout << std::endl << std::endl;
    std::cout << "Test complete!" << std::endl;
    std::cout << "Total frames processed: " << audioEngine.getTotalFramesProcessed() << std::endl;
    std::cout << std::endl;

    // Cleanup
    std::cout << "Shutting down..." << std::endl;
    samplePlayer.stop();
    midiManager.shutdown();
    audioEngine.shutdown();

    std::cout << "Drum Machine shutdown cleanly" << std::endl;
    return 0;
}
