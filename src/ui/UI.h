#ifndef UI_H
#define UI_H

#include <cstdint>
#include <memory>

namespace DrumMachine {

class Sequencer;
class AssetManager;

/**
 * UI
 * 
 * Immediate-mode GUI using Dear ImGui.
 * Renders step grid, transport controls, track strips.
 * Milestone 3: Minimal ImGui integration
 * Milestone 4: Custom graphics integration
 */
class UI {
public:
    UI(uint32_t windowWidth = 1280, uint32_t windowHeight = 720);
    ~UI();

    // Initialize ImGui and create window
    bool initialize(Sequencer* sequencer);

    // Shutdown UI
    void shutdown();

    // Render frame
    void render();

    // Handle input
    void handleInput();

    // Check if window should close
    bool shouldClose() const;

    // Set asset manager for custom graphics
    void setAssetManager(AssetManager* assetManager);

private:
    uint32_t windowWidth_;
    uint32_t windowHeight_;
    Sequencer* sequencer_;
    AssetManager* assetManager_;
    void* windowHandle_; // SDL2 window pointer

    // Render sections
    void renderTransportSection();
    void renderStepGrid();
    void renderTrackStrips();
    void renderMetronomeSection();
};

} // namespace DrumMachine

#endif // UI_H
