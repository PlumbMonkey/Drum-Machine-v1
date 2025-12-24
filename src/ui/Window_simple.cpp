#include "Window.h"
#include "StepEditor.h"
#include "../audio/AudioEngine.h"
#include "../sequencer/Sequencer.h"
#include <SDL2/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

namespace DrumMachine {

void Window::renderUI()
{
    // Create ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Clear background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Main menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "ESC")) {
                isOpen_ = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Transport window - fixed position
    {
        ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 120), ImGuiCond_FirstUseEver);
        
        if (ImGui::Begin("Transport")) {
            static float tempo = 120.0f;
            static float swing = 0.0f;

            ImGui::SliderFloat("Tempo (BPM)", &tempo, 60.0f, 180.0f);
            ImGui::SliderFloat("Swing", &swing, 0.0f, 0.6f, "%.2f");

            if (sequencer_) {
                sequencer_->getTransport().setTempo(tempo);
                sequencer_->getTransport().setSwing(swing);
            }

            // Calculate current step from audio position
            if (audioEngine_ && sequencer_) {
                uint64_t frameCount = audioEngine_->getTotalFramesProcessed();
                uint32_t sampleRate = audioEngine_->getSampleRate();
                float currentTempo = sequencer_->getTransport().getTempo();
                uint64_t samplesPerBeat = (sampleRate * 60) / static_cast<uint32_t>(currentTempo);
                uint64_t samplesPerStep = samplesPerBeat / 4;
                if (samplesPerStep > 0) {
                    currentStep_ = (frameCount / samplesPerStep) % 16;
                }
                ImGui::Text("Step: %u", currentStep_);
            }

            ImGui::End();
        }
    }

    // Step Editor window - main grid
    {
        ImGui::SetNextWindowPos(ImVec2(10, 160), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1200, 450), ImGuiCond_FirstUseEver);
        
        if (stepEditor_ && sequencer_) {
            stepEditor_->render(sequencer_, currentStep_);
        }
    }

    // End ImGui frame
    ImGui::Render();
}

} // namespace DrumMachine
