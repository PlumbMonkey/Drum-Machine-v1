#include "StepEditor.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Pattern.h"
#include <imgui.h>

namespace DrumMachine {

StepEditor::StepEditor()
    : selectedTrack_(0)
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
    if (ImGui::Begin("Step Editor", nullptr, ImGuiWindowFlags_NoMove)) {
        ImGui::Columns(2, "step_editor_columns", true);

        // Left panel: track controls
        renderTrackPanel(currentStep);

        ImGui::NextColumn();

        // Right panel: step grid
        renderStepGrid(sequencer, currentStep);

        ImGui::Columns(1);
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

    // Step numbers header
    ImGui::Spacing();
    ImGui::Text("Step: ");
    ImGui::SameLine();
    for (uint32_t step = 0; step < NUM_STEPS; ++step) {
        ImGui::SameLine();
        ImGui::Text("%d", step);
        if (step < NUM_STEPS - 1) ImGui::SameLine();
    }

    // Step buttons grid
    ImGui::Spacing();
    float buttonSize = 30.0f;
    ImGui::PushButtonRepeat(false);

    for (uint32_t track = 0; track < NUM_TRACKS; ++track) {
        // Track label
        ImGui::Text("%s", trackNames_[track]);
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        // Step buttons for this track
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

            // Draw button
            if (ImGui::Button("##step", ImVec2(buttonSize, buttonSize))) {
                // Toggle step
                pattern.setStepActive(track, step, !isEnabled);
            }

            ImGui::PopStyleColor(3);

            // Tooltip showing step number
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Track %d, Step %d", track, step);
            }

            ImGui::PopID();

            // Add spacing between buttons, new line after 16 steps
            if (step < NUM_STEPS - 1) {
                ImGui::SameLine();
            }
        }
    }

    ImGui::PopButtonRepeat();
}

} // namespace DrumMachine
