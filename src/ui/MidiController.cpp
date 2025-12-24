#include "MidiController.h"
#include "../audio/MidiManager.h"
#include <imgui.h>
#include <iostream>
#include <chrono>

namespace DrumMachine {

MidiController::MidiController()
    : selectedPortIndex_(-1), showPortList_(false), lastMidiMessageTime_(0)
{
}

void MidiController::render(MidiManager* midiManager)
{
    if (!ImGui::Begin("MIDI Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }

    if (!midiManager) {
        ImGui::Text("MIDI Manager not initialized.");
        ImGui::End();
        return;
    }

    ImGui::Text("MIDI Input");
    ImGui::Separator();

    // Connection status
    bool isActive = midiManager->isActive();
    ImGui::TextColored(
        isActive ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        isActive ? "Connected" : "Disconnected"
    );

    ImGui::Spacing();

    // Available ports
    std::vector<std::string> ports = midiManager->getInputPorts();

    ImGui::Text("Available MIDI Ports:");
    ImGui::Separator();

    if (ports.empty()) {
        ImGui::Text("No MIDI ports found.");
    } else {
        for (size_t i = 0; i < ports.size(); ++i) {
            bool isSelected = (selectedPortIndex_ == static_cast<int>(i));
            if (ImGui::Selectable(ports[i].c_str(), isSelected)) {
                selectedPortIndex_ = static_cast<int>(i);
                if (midiManager->openPort(i)) {
                    std::cout << "Connected to MIDI port: " << ports[i] << std::endl;
                }
            }
        }
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Virtual port option
    if (ImGui::Button("Create Virtual Port", ImVec2(200, 0))) {
        if (midiManager->openVirtualPort("Drum Machine Input")) {
            ImGui::OpenPopup("Virtual Port Created");
        } else {
            ImGui::OpenPopup("Virtual Port Error");
        }
    }

    ImGui::Spacing();

    // MIDI Activity Monitor
    ImGui::Text("MIDI Activity:");
    ImGui::Separator();

    // Process messages
    uint32_t messagesProcessed = midiManager->processMessages();
    if (messagesProcessed > 0) {
        auto now = std::chrono::steady_clock::now();
        lastMidiMessageTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
    }

    ImGui::Text("Messages processed this frame: %u", messagesProcessed);

    // Display CC values (real-time monitor)
    ImGui::Text("CC Monitor:");
    ImGui::Spacing();

    const char* ccLabels[] = {
        "Bank", "Mod Wheel", "Breath", "Undefined", "Foot", "Portamento Time",
        "Data MSB", "Channel Volume", "Balance", "Undefined", "Pan", "Expression",
        "Effect Ctrl 1", "Effect Ctrl 2"
    };

    for (int i = 0; i < 14; ++i) {
        uint8_t value = midiManager->getControlValue(i);
        if (value > 0) {
            ImGui::Text("CC %d (%s): %d", i, ccLabels[i], value);
        }
    }

    // Popups
    if (ImGui::BeginPopupModal("Virtual Port Created", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Virtual MIDI port created successfully!");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Virtual Port Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Virtual MIDI ports not supported on this platform.\nUse physical MIDI device instead.");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace DrumMachine
