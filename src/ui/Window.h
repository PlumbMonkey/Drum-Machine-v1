#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <string>
#include <cstdint>

// Forward declarations for SDL - will be fully defined in .cpp
namespace DrumMachine {

class AudioEngine;
class Sequencer;
class StepEditor;
class PatternManager;
class MidiManager;

/**
 * Window
 * 
 * Manages SDL2 window creation, OpenGL context, and ImGui integration.
 * Handles event loop and rendering.
 */
class Window {
public:
    Window(uint32_t width = 1280, uint32_t height = 720);
    ~Window();

    // Initialize SDL2, OpenGL context, ImGui
    bool initialize();

    // Shutdown and cleanup resources
    void shutdown();

    // Process one frame: input, ImGui rendering, OpenGL swap
    bool processFrame();

    // Is window still open?
    bool isOpen() const { return isOpen_; }

    // Set references to audio/sequencer for UI control
    void setAudioEngine(AudioEngine* audioEngine) { audioEngine_ = audioEngine; }
    void setSequencer(Sequencer* sequencer) { sequencer_ = sequencer; }
    void setMidiManager(MidiManager* midiManager) { midiManager_ = midiManager; }

    // Getters
    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }

private:
    uint32_t width_;
    uint32_t height_;
    bool isOpen_;

    void* sdlWindow_;   // SDL_Window*
    void* glContext_;   // SDL_GLContext

    AudioEngine* audioEngine_;
    Sequencer* sequencer_;
    MidiManager* midiManager_;

    // UI components
    std::unique_ptr<StepEditor> stepEditor_;
    std::unique_ptr<PatternManager> patternManager_;

    // ImGui state
    char patternNameBuffer_[256];
    uint32_t currentStep_;  // Current playhead position for visualization

    // Internal methods
    void handleEvents();
    void renderUI();
    void renderFrame();
};

} // namespace DrumMachine

#endif // WINDOW_H
