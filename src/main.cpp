#include "DrumMachine.h"
#include "audio/AudioEngine.h"
#include "audio/SamplePlayer.h"
#include "sequencer/Sequencer.h"
#include "sequencer/Transport.h"
#include "ui/Window.h"
#include <iostream>

using namespace DrumMachine;

/**
 * Milestone 2: UI Scaffolding
 * 
 * Goal: Create interactive ImGui interface with audio engine running in background.
 * - SDL2 window with OpenGL context
 * - ImGui immediate-mode interface
 * - Transport controls (play/pause, tempo, time signature)
 * - Real-time audio playback
 */

int main(int argc, char* argv[])
{
    std::cout << "======================================" << std::endl;
    std::cout << "Drum Machine v" << DRUM_MACHINE_VERSION << std::endl;
    std::cout << "Milestone 2: UI Scaffolding" << std::endl;
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

    // Initialize window and UI
    std::cout << "[4/4] Initializing UI..." << std::endl;
    Window window(1280, 720);
    if (!window.initialize()) {
        std::cerr << "FAILED to initialize window" << std::endl;
        audioEngine.shutdown();
        return 1;
    }
    window.setAudioEngine(&audioEngine);
    window.setSequencer(&sequencer);
    std::cout << "      UI OK" << std::endl;
    std::cout << std::endl;

    std::cout << "Starting main event loop..." << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    std::cout << std::endl;

    // Main loop
    while (window.isOpen()) {
        if (!window.processFrame()) {
            break;
        }
    }

    std::cout << std::endl;
    std::cout << "Shutting down..." << std::endl;

    // Cleanup
    samplePlayer.stop();
    window.shutdown();
    audioEngine.shutdown();

    std::cout << "Drum Machine shutdown cleanly" << std::endl;
    return 0;
}
