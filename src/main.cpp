#include "include/DrumMachine.h"
#include "audio/AudioEngine.h"
#include "sequencer/Sequencer.h"
#include "ui/UI.h"
#include <iostream>

using namespace DrumMachine;

int main(int argc, char* argv[])
{
    std::cout << "Drum Machine v" << DRUM_MACHINE_VERSION << std::endl;

    // Initialize audio engine
    AudioEngine audioEngine(44100);
    if (!audioEngine.initialize()) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
        return 1;
    }

    // Initialize sequencer
    Sequencer sequencer(44100);
    audioEngine.setSequencer(&sequencer);

    // Initialize UI
    UI ui(1280, 720);
    if (!ui.initialize(&sequencer)) {
        std::cerr << "Failed to initialize UI" << std::endl;
        return 1;
    }

    // Main loop
    std::cout << "Starting main loop..." << std::endl;
    while (!ui.shouldClose()) {
        ui.handleInput();
        ui.render();
        sequencer.advanceFrame(256); // Process 256 frames per UI frame
    }

    // Cleanup
    ui.shutdown();
    audioEngine.shutdown();

    std::cout << "Drum Machine shutdown cleanly" << std::endl;
    return 0;
}
