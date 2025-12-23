#include "DrumMachine.h"
#include "audio/AudioEngine.h"
#include "audio/SamplePlayer.h"
#include "sequencer/Sequencer.h"
#include "sequencer/Transport.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace DrumMachine;

/**
 * Milestone 1: Audio Proof-of-Concept
 * 
 * Goal: Play one sample on a loop at 120 BPM with verified timing.
 * - Load a WAV sample
 * - Start audio engine
 * - Play for 5+ minutes
 * - Verify no timing drift
 */

int main(int argc, char* argv[])
{
    std::cout << "======================================" << std::endl;
    std::cout << "Drum Machine v" << DRUM_MACHINE_VERSION << std::endl;
    std::cout << "Milestone 1: Audio Proof-of-Concept" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;

    // Configuration
    uint32_t sampleRate = 44100;
    float tempoInBPM = 120.0f;
    std::string samplePath = "assets/samples/test_kick.wav";

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
    
    // Set transport parameters
    sequencer.getTransport().setTempo(tempoInBPM);
    sequencer.getTransport().setTimeSignature("4/4");
    sequencer.getTransport().setBarCount(1);
    sequencer.getTransport().play();
    
    std::cout << "      Sequencer OK" << std::endl;
    std::cout << "      Tempo: " << tempoInBPM << " BPM" << std::endl;
    std::cout << std::endl;

    // Load sample
    std::cout << "[3/5] Loading sample..." << std::endl;
    SamplePlayer samplePlayer(sampleRate);
    if (!samplePlayer.loadSample(samplePath)) {
        std::cerr << "FAILED to load sample: " << samplePath << std::endl;
        audioEngine.shutdown();
        return 1;
    }
    std::cout << "      Sample OK (" << std::fixed << std::setprecision(3) 
              << samplePlayer.getDurationSeconds() << " seconds)" << std::endl;
    std::cout << std::endl;

    // Start sample playback
    std::cout << "[4/5] Starting audio playback..." << std::endl;
    samplePlayer.start();
    std::cout << "      Playback OK" << std::endl;
    std::cout << std::endl;

    // Monitor playback for 5+ minutes
    std::cout << "[5/5] Monitoring playback for timing drift..." << std::endl;
    std::cout << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();
    uint32_t checkIntervalSeconds = 30;  // Check every 30 seconds
    uint32_t totalDurationSeconds = 300; // Run for 5 minutes (300 seconds)
    
    std::cout << std::setw(12) << "Time (s)" << " | "
              << std::setw(18) << "Frames Processed" << " | "
              << std::setw(16) << "Expected Frames" << " | "
              << std::setw(10) << "Drift (ms)" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - startTime).count();
        
        float elapsedSeconds = elapsedTime / 1000.0f;

        if (elapsedSeconds > totalDurationSeconds) {
            std::cout << std::endl << "Test complete!" << std::endl;
            break;
        }

        // Every 30 seconds, check timing
        static uint32_t lastCheckTime = 0;
        if (static_cast<uint32_t>(elapsedSeconds) % checkIntervalSeconds == 0 &&
            static_cast<uint32_t>(elapsedSeconds) > lastCheckTime) {
            
            lastCheckTime = static_cast<uint32_t>(elapsedSeconds);

            uint64_t framesProcessed = audioEngine.getTotalFramesProcessed();
            uint64_t expectedFrames = static_cast<uint64_t>(elapsedSeconds * sampleRate);
            
            // Calculate drift in milliseconds
            int64_t frameDiff = static_cast<int64_t>(framesProcessed) - 
                               static_cast<int64_t>(expectedFrames);
            float driftMs = (frameDiff / static_cast<float>(sampleRate)) * 1000.0f;

            std::cout << std::setw(12) << std::fixed << std::setprecision(1) << elapsedSeconds << " | "
                      << std::setw(18) << framesProcessed << " | "
                      << std::setw(16) << expectedFrames << " | "
                      << std::setw(10) << std::setprecision(2) << driftMs << " ms" << std::endl;
        }

        // Sleep briefly to avoid busy-wait
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Print final statistics
    std::cout << std::endl << "Final Statistics:" << std::endl;
    std::cout << "================" << std::endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();
    float totalElapsedSeconds = totalElapsed / 1000.0f;
    
    uint64_t totalFramesProcessed = audioEngine.getTotalFramesProcessed();
    uint64_t expectedTotalFrames = static_cast<uint64_t>(totalElapsedSeconds * sampleRate);
    int64_t totalDriftFrames = static_cast<int64_t>(totalFramesProcessed) - 
                              static_cast<int64_t>(expectedTotalFrames);
    float totalDriftMs = (totalDriftFrames / static_cast<float>(sampleRate)) * 1000.0f;

    std::cout << "Total elapsed time: " << std::fixed << std::setprecision(2) 
              << totalElapsedSeconds << " seconds" << std::endl;
    std::cout << "Frames processed:   " << totalFramesProcessed << std::endl;
    std::cout << "Expected frames:    " << expectedTotalFrames << std::endl;
    std::cout << "Total drift:        " << std::setprecision(3) << totalDriftMs << " ms" 
              << " (" << std::setprecision(4) << (totalDriftMs / totalElapsedSeconds) 
              << " ms/sec)" << std::endl;

    if (std::abs(totalDriftMs) < 1.0f) {
        std::cout << std::endl << "✓ SUCCESS: Timing is accurate (< 1ms drift)" << std::endl;
    } else if (std::abs(totalDriftMs) < 10.0f) {
        std::cout << std::endl << "~ OK: Timing is acceptable (< 10ms drift)" << std::endl;
    } else {
        std::cout << std::endl << "✗ FAILED: Timing drift too large (> 10ms)" << std::endl;
    }

    std::cout << std::endl;

    // Cleanup
    samplePlayer.stop();
    audioEngine.shutdown();

    std::cout << "Drum Machine shutdown cleanly" << std::endl;
    return 0;
}
