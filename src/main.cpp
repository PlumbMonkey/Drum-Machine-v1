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
    
    // Track names and sample files (8 drum kit)
    const char* trackNames[8] = {
        "Kick",
        "Snare",
        "Closed Hi-Hat",
        "Open Hi-Hat",
        "Tom High",
        "Tom Mid",
        "Tom Low",
        "Ride"
    };
    
    const char* sampleFiles[8] = {
        "kick.wav",
        "snare.wav",
        "closed_hihat.wav",
        "open_hihat.wav",
        "tom_high.wav",
        "tom_mid.wav",
        "tom_low.wav",
        "ride.wav"
    };

    // Initialize audio engine
    std::cout << "[1/5] Initializing audio engine..." << std::endl;
    AudioEngine audioEngine(sampleRate);
    if (!audioEngine.initialize()) {
        std::cerr << "FAILED to initialize audio engine" << std::endl;
        return 1;
    }
    std::cout << "      Audio engine OK" << std::endl;
    std::cout << std::endl;

    // Initialize sequencer
    std::cout << "[2/5] Initializing sequencer..." << std::endl;
    Sequencer sequencer(sampleRate);
    audioEngine.setSequencer(&sequencer);
    sequencer.getTransport().setTempo(120.0f);
    sequencer.getTransport().setTimeSignature("4/4");
    sequencer.getTransport().setBarCount(1);
    std::cout << "      Sequencer OK" << std::endl;
    std::cout << std::endl;

    // Load 8 drum samples (one per track)
    std::cout << "[3/5] Loading drum kit samples..." << std::endl;
    std::vector<std::unique_ptr<SamplePlayer>> samplePlayers;
    std::vector<SamplePlayer*> rawPlayerPtrs;
    
    for (int track = 0; track < 8; ++track) {
        auto player = std::make_unique<SamplePlayer>(sampleRate);
        if (!player->loadSample(sampleFiles[track])) {
            std::cerr << "WARNING: Failed to load sample: " << sampleFiles[track] << std::endl;
            // Continue - allow other samples to load
        } else {
            std::cout << "      ✓ " << trackNames[track] << " (" << sampleFiles[track] << ")" << std::endl;
        }
        rawPlayerPtrs.push_back(player.get());
        samplePlayers.push_back(std::move(player));
    }
    
    // Wire all sample players to audio engine
    for (int track = 0; track < 8; ++track) {
        audioEngine.setSamplePlayer(track, rawPlayerPtrs[track]);
    }
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
    window.setSamplePlayer(rawPlayerPtrs[0]);  // Keep reference for backwards compatibility
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
    for (auto& player : samplePlayers) {
        player->stop();
    }
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
