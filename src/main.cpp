#include "DrumMachine.h"
#include "audio/AudioEngine.h"
#include "audio/SamplePlayer.h"
#include "audio/MidiManager.h"
#include "sequencer/Sequencer.h"
#include "sequencer/Transport.h"
#include "ui/Window.h"
#include <iostream>
#include <fstream>

using namespace DrumMachine;

/**
 * Milestone 5: MIDI Foundation
 * 
 * Goal: Add MIDI input support for external controllers.
 * - RtMidi integration for MIDI port management
 * - MIDI note triggering for drum pads
 * - MIDI CC parameter control
 * - Event publishing via ParameterBus
 */

int main(int argc, char* argv[])
{
    try {
    // Open error log file for Windows GUI apps that lose console output
    std::ofstream logFile("DrumMachine_error.log", std::ios::app);
    
    std::cout << "======================================" << std::endl;
    std::cout << "Drum Machine v" << DRUM_MACHINE_VERSION << std::endl;
    std::cout << "Milestone 5: MIDI Foundation" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;
    
    logFile << "Drum Machine started" << std::endl;
    logFile.flush();

    // Configuration
    uint32_t sampleRate = 44100;
    std::string samplePath = "test_kick.wav";  // Look for WAV in exe directory or assets/samples/

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
    audioEngine.setSamplePlayer(&samplePlayer);  // Wire sample player to audio engine
    std::cout << std::endl;

    // Initialize MIDI manager
    std::cout << "[4/5] Initializing MIDI..." << std::endl;
    MidiManager midiManager;
    if (!midiManager.initialize()) {
        std::cerr << "WARNING: MIDI initialization failed" << std::endl;
    }
    std::cout << "      MIDI OK" << std::endl;
    std::cout << std::endl;

    // Initialize window and UI
    std::cout << "[5/5] Initializing UI..." << std::endl;
    Window window(1280, 720);
    if (!window.initialize()) {
        std::cerr << "FAILED to initialize window" << std::endl;
        logFile << "ERROR: Failed to initialize window" << std::endl;
        logFile.close();
        audioEngine.shutdown();
        return 1;
    }
    window.setAudioEngine(&audioEngine);
    window.setSequencer(&sequencer);
    window.setMidiManager(&midiManager);
    window.setSamplePlayer(&samplePlayer);
    std::cout << "      UI OK" << std::endl;
    std::cout << std::endl;

    std::cout << "Starting main event loop..." << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    std::cout << std::endl;

    // Main loop
    int frameCount = 0;
    while (window.isOpen()) {
        frameCount++;
        if (frameCount <= 5 || frameCount % 100 == 0) {
            std::cout << "Frame " << frameCount << ", isOpen: " << window.isOpen() << std::endl;
        }
        if (!window.processFrame()) {
            std::cout << "processFrame returned false" << std::endl;
            break;
        }
    }

    std::cout << std::endl;
    std::cout << "Shutting down..." << std::endl;

    // Cleanup
    samplePlayer.stop();
    window.shutdown();
    midiManager.shutdown();
    audioEngine.shutdown();

    std::cout << "Drum Machine shutdown cleanly" << std::endl;
    logFile << "Shutdown successful" << std::endl;
    logFile.close();
    return 0;
    } catch (const std::exception& e) {
        std::ofstream logFile("DrumMachine_error.log", std::ios::app);
        logFile << "EXCEPTION: " << e.what() << std::endl;
        logFile.close();
        return 1;
    } catch (...) {
        std::ofstream logFile("DrumMachine_error.log", std::ios::app);
        logFile << "UNKNOWN EXCEPTION" << std::endl;
        logFile.close();
        return 1;
    }
}
