#ifndef STEP_EDITOR_H
#define STEP_EDITOR_H

#include <cstdint>
#include <string>
#include <array>

namespace DrumMachine {

class Sequencer;
class SamplePlayer;

/**
 * StepEditor
 * 
 * Immediate-mode UI for 16-step drum pattern editor.
 * Displays 8 drum tracks with 16 steps each.
 * - Left panel: track controls (mute, solo, labels)
 * - Center: 16-step grid (click to toggle)
 * - Visual feedback: playhead position, active steps
 */
class StepEditor {
public:
    StepEditor();

    // Render the step editor UI
    void render(Sequencer* sequencer, uint32_t currentStep);

    // Set sample player for triggering on pad clicks
    void setSamplePlayer(SamplePlayer* samplePlayer) { samplePlayer_ = samplePlayer; }
    
    // Set all 8 sample players for multi-track pad preview
    void setSamplePlayers(const std::array<SamplePlayer*, 8>& players);

    // Get/set selected track
    uint32_t getSelectedTrack() const { return selectedTrack_; }
    void setSelectedTrack(uint32_t track) { selectedTrack_ = track; }

    // Track properties
    bool isTrackMuted(uint32_t track) const { return mutedTracks_[track]; }
    void setTrackMuted(uint32_t track, bool muted) { mutedTracks_[track] = muted; }

private:
    static constexpr uint32_t NUM_TRACKS = 8;
    static constexpr uint32_t NUM_STEPS = 16;

    uint32_t selectedTrack_;
    std::array<bool, NUM_TRACKS> mutedTracks_;
    SamplePlayer* samplePlayer_;  // For triggering samples on pad clicks
    std::array<SamplePlayer*, NUM_TRACKS> samplePlayers_;  // All 8 sample players for pad preview

    // Track display names
    const char* trackNames_[NUM_TRACKS] = {
        "Kick",
        "Snare",
        "Hi-Hat Closed",
        "Hi-Hat Open",
        "Tom High",
        "Tom Mid",
        "Tom Low",
        "Cowbell"
    };

    // Render left panel with track controls
    void renderTrackPanel(uint32_t currentStep);

    // Render center grid
    void renderStepGrid(Sequencer* sequencer, uint32_t currentStep);
};

} // namespace DrumMachine

#endif // STEP_EDITOR_H
