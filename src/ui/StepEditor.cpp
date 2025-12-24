#include "StepEditor.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Pattern.h"
#include "../audio/SamplePlayer.h"
#include <imgui.h>

namespace DrumMachine {

StepEditor::StepEditor()
    : selectedTrack_(0), samplePlayer_(nullptr)
{
    // Initialize all tracks as unmuted
    for (auto& muted : mutedTracks_) {
        muted = false;
    }
}

void StepEditor::render(Sequencer* sequencer, uint32_t currentStep)
{
    if (!sequencer) return;

    ImGui::SetNextWindowSize(ImVec2(1200, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Step Editor")) {
        // Don't use columns - just render the grid directly
        renderStepGrid(sequencer, currentStep);
        ImGui::End();
    }
}

void StepEditor::renderTrackPanel(uint32_t currentStep)
{
    ImGui::Text("Tracks");
    ImGui::Separator();

    for (uint32_t track = 0; track < NUM_TRACKS; ++track) {
        ImGui::PushID(track);

        // Track selection
        bool isSelected = (track == selectedTrack_);
        if (ImGui::Selectable(trackNames_[track], isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
            selectedTrack_ = track;
        }

        ImGui::SameLine();

        // Mute button
        bool muted = mutedTracks_[track];
        if (ImGui::SmallButton(muted ? "M##mute" : "M")) {
            mutedTracks_[track] = !mutedTracks_[track];
        }

        ImGui::PopID();
    }
}

void StepEditor::renderStepGrid(Sequencer* sequencer, uint32_t currentStep)
{
    if (!sequencer) return;

    Pattern& pattern = sequencer->getPattern();

    ImGui::Text("Steps");
    ImGui::Separator();

    // Step numbers header with proper alignment
    ImGui::Spacing();
    float buttonSize = 30.0f;
    float spacingX = 4.0f;
    float labelWidth = 80.0f;

    // Header row
    ImGui::Text("Track");
    ImGui::SameLine(labelWidth);
    for (uint32_t step = 0; step < NUM_STEPS; ++step) {
        ImGui::Text("%d", step);
        ImGui::SameLine(labelWidth + (step + 1) * (buttonSize + spacingX));
    }
    ImGui::NewLine();

    // Step buttons grid with fixed positioning
    ImGui::PushButtonRepeat(false);

    for (uint32_t track = 0; track < NUM_TRACKS; ++track) {
        // Track label
        ImGui::Text("%s", trackNames_[track]);
        ImGui::SameLine(labelWidth);

        // Step buttons for this track
        float xPos = labelWidth;
        for (uint32_t step = 0; step < NUM_STEPS; ++step) {
            ImGui::PushID(track * NUM_STEPS + step);

            // Check if this step is enabled
            bool isEnabled = pattern.isStepActive(track, step);

            // Visual feedback: different color for current step
            bool isCurrent = (step == currentStep);
            ImVec4 buttonColor = isEnabled ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)    // Green
                                           : ImVec4(0.3f, 0.3f, 0.3f, 1.0f);   // Dark grey

            if (isCurrent) {
                buttonColor = isEnabled ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)      // Yellow
                                        : ImVec4(0.6f, 0.6f, 0.0f, 1.0f);    // Dark yellow
            }

            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));

            // Draw button at fixed position
            ImGui::SetCursorPosX(xPos);
            if (ImGui::Button("##step", ImVec2(buttonSize, buttonSize))) {
                // Toggle step in pattern
                pattern.setStepActive(track, step, !isEnabled);
            }

            ImGui::PopStyleColor(3);

            // Tooltip showing step number
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Track %d, Step %d", track, step);
            }

            ImGui::PopID();
            xPos += buttonSize + spacingX;

            if (step < NUM_STEPS - 1) {
                ImGui::SameLine();
            }
        }
    }

    ImGui::PopButtonRepeat();
}

} // namespace DrumMachine
