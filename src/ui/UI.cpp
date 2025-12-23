#include "UI.h"
#include <iostream>

namespace DrumMachine {

UI::UI(uint32_t windowWidth, uint32_t windowHeight)
    : windowWidth_(windowWidth), windowHeight_(windowHeight), 
      sequencer_(nullptr), assetManager_(nullptr), windowHandle_(nullptr)
{
}

UI::~UI()
{
    shutdown();
}

bool UI::initialize(Sequencer* sequencer)
{
    sequencer_ = sequencer;

    // TODO: Initialize SDL2 window
    // TODO: Initialize OpenGL context
    // TODO: Initialize ImGui

    std::cout << "UI initialized at " << windowWidth_ << "x" << windowHeight_ << std::endl;
    return true;
}

void UI::shutdown()
{
    // TODO: Cleanup ImGui
    // TODO: Cleanup SDL2 window and OpenGL context
}

void UI::render()
{
    // TODO: Begin ImGui frame
    // TODO: Call render sections
    // TODO: End ImGui frame and present
}

void UI::handleInput()
{
    // TODO: Process SDL2 events
}

bool UI::shouldClose() const
{
    // TODO: Check if window close event occurred
    return false;
}

void UI::setAssetManager(AssetManager* assetManager)
{
    assetManager_ = assetManager;
}

void UI::renderTransportSection()
{
    // TODO: ImGui controls for:
    // - Play / Stop button
    // - Tempo slider (60-180 BPM)
    // - Swing slider (0-60%)
    // - Time signature selector
    // - Bar count selector
}

void UI::renderStepGrid()
{
    // TODO: ImGui step grid (8 x 16 buttons)
    // - Custom step button graphics
    // - Playhead indicator
    // - Inactive steps visually dimmed
}

void UI::renderTrackStrips()
{
    // TODO: ImGui controls for each of 8 tracks:
    // - Track name label
    // - Volume slider
    // - Mute button
    // - Sample load button
}

void UI::renderMetronomeSection()
{
    // TODO: ImGui controls for:
    // - On/Off toggle
    // - Tone selector (Click, Stick, Rim)
    // - Volume slider
}

} // namespace DrumMachine
