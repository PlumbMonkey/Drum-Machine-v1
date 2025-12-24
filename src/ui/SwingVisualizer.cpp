#include "SwingVisualizer.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Transport.h"
#include <imgui.h>

namespace DrumMachine {

SwingVisualizer::SwingVisualizer()
{
}

float SwingVisualizer::calculateStepOffset(Sequencer* sequencer, uint32_t stepIndex)
{
    if (!sequencer) return 0.0f;

    Transport& transport = sequencer->getTransport();
    
    // Swing applies to even-numbered steps (1, 3, 5, etc. in 0-based indexing)
    if (stepIndex % 2 == 1) {
        float swingAmount = transport.getSwing();
        // swingAmount is 0.0 to 0.6, scale to percentage offset (0-60% of step)
        return swingAmount * 100.0f;
    }

    return 0.0f;
}

void SwingVisualizer::render(Sequencer* sequencer)
{
    if (!sequencer) return;

    if (ImGui::Begin("Swing Visualization", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        Transport& transport = sequencer->getTransport();
        float swingAmount = transport.getSwing();

        // Title and swing amount display
        ImGui::Text("Swing Amount: %.2f (%.0f%%)", swingAmount, swingAmount * 100.0f);
        
        ImGui::Spacing();
        ImGui::Separator();

        // Draw timeline of 16 steps with swing offsets
        ImGui::Text("Step Timing Offsets:");
        ImGui::Spacing();

        const float stepWidth = 40.0f;
        const float stepHeight = 20.0f;
        const float barHeight = 60.0f;
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        // Draw background grid
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // Draw step boxes and offsets
        for (uint32_t i = 0; i < 16; ++i) {
            float xPos = cursorPos.x + (i * stepWidth);
            float yPos = cursorPos.y;

            // Determine colors
            ImU32 boxColor = (i % 2 == 0) ? 
                ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 1.0f)) :  // Even steps (on beat) - blue
                ImGui::GetColorU32(ImVec4(0.8f, 0.3f, 0.3f, 1.0f));   // Odd steps (swing steps) - red

            // Draw step box
            ImVec2 boxMin(xPos, yPos);
            ImVec2 boxMax(xPos + stepWidth - 2.0f, yPos + stepHeight);
            drawList->AddRect(boxMin, boxMax, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.5f)));
            drawList->AddRectFilled(boxMin, boxMax, boxColor);

            // Draw step number
            ImGui::SetCursorScreenPos(ImVec2(xPos + 5.0f, yPos + 2.0f));
            ImGui::Text("%u", i);
        }

        // Add spacing for text below
        ImGui::Dummy(ImVec2(stepWidth * 16.0f, stepHeight + 5.0f));

        ImGui::Spacing();
        ImGui::Separator();

        // Draw offset bars
        ImGui::Text("Timing Offset Visualization:");
        ImGui::Spacing();

        cursorPos = ImGui::GetCursorScreenPos();

        // Draw horizontal timeline with step offsets
        for (uint32_t i = 0; i < 16; ++i) {
            float offsetPercent = calculateStepOffset(sequencer, i);
            float xPos = cursorPos.x + (i * stepWidth);
            float yPos = cursorPos.y;

            // Draw baseline
            ImVec2 baselineStart(xPos + 5.0f, yPos + barHeight);
            ImVec2 baselineEnd(xPos + stepWidth - 7.0f, yPos + barHeight);
            drawList->AddLine(baselineStart, baselineEnd, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.5f)));

            // Draw offset bar (if any)
            if (offsetPercent > 0.0f) {
                float barWidth = (offsetPercent / 100.0f) * (stepWidth - 12.0f);
                ImVec2 barStart(xPos + 5.0f, yPos + barHeight - 3.0f);
                ImVec2 barEnd(xPos + 5.0f + barWidth, yPos + barHeight + 3.0f);
                drawList->AddRectFilled(barStart, barEnd, ImGui::GetColorU32(ImVec4(0.2f, 0.9f, 0.2f, 0.8f)));
            }

            // Draw step label
            ImGui::SetCursorScreenPos(ImVec2(xPos + 8.0f, yPos + barHeight + 8.0f));
            ImGui::Text("%u", i);
        }

        // Add spacing for labels below
        ImGui::Dummy(ImVec2(stepWidth * 16.0f, barHeight + 30.0f));

        ImGui::Spacing();
        ImGui::Separator();

        // Legend
        ImGui::Text("Legend:");
        ImGui::BulletText("Blue boxes: On-beat steps (not affected by swing)");
        ImGui::BulletText("Red boxes: Swing-affected steps (delayed timing)");
        ImGui::BulletText("Green bars: Amount of timing delay");

        ImGui::End();
    }
}

} // namespace DrumMachine
