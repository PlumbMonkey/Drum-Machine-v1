#include "Window.h"
#include "StepEditor.h"
#include "PatternManager.h"
#include "SwingVisualizer.h"
#include "SampleBrowser.h"
#include "../audio/AudioEngine.h"
#include "../sequencer/Sequencer.h"
#include <SDL2/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <cstring>

namespace DrumMachine {

Window::Window(uint32_t width, uint32_t height)
    : width_(width), height_(height), isOpen_(true),
      sdlWindow_(nullptr), glContext_(nullptr),
      audioEngine_(nullptr), sequencer_(nullptr),
      showDemoWindow_(false), showSaveDialog_(false), 
      showLoadDialog_(false), currentStep_(0)
{
    stepEditor_ = std::make_unique<StepEditor>();
    patternManager_ = std::make_unique<PatternManager>();
    swingVisualizer_ = std::make_unique<SwingVisualizer>();
    sampleBrowser_ = std::make_unique<SampleBrowser>();
    std::memset(patternNameBuffer_, 0, sizeof(patternNameBuffer_));
}

Window::~Window()
{
    shutdown();
}

bool Window::initialize()
{
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create SDL window with OpenGL context
    sdlWindow_ = SDL_CreateWindow(
        "Drum Machine v0.1.0",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width_, height_,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!sdlWindow_) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    glContext_ = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(sdlWindow_));
    if (!glContext_) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(sdlWindow_));
        SDL_Quit();
        return false;
    }

    SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(sdlWindow_), 
                      reinterpret_cast<SDL_GLContext>(glContext_));
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup ImGui backends
    ImGui_ImplSDL2_InitForOpenGL(reinterpret_cast<SDL_Window*>(sdlWindow_), glContext_);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "Window initialized successfully" << std::endl;
    return true;
}

void Window::shutdown()
{
    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Destroy OpenGL context
    if (glContext_) {
        SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(glContext_));
        glContext_ = nullptr;
    }

    // Destroy SDL window
    if (sdlWindow_) {
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(sdlWindow_));
        sdlWindow_ = nullptr;
    }

    SDL_Quit();
    std::cout << "Window shutdown" << std::endl;
}

void Window::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                isOpen_ = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isOpen_ = false;
                }
                break;
            default:
                break;
        }
    }
}

void Window::renderUI()
{
    // Create ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Main menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Pattern", "Ctrl+S")) {
                showSaveDialog_ = true;
            }
            if (ImGui::MenuItem("Load Pattern", "Ctrl+O")) {
                showLoadDialog_ = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ESC")) {
                isOpen_ = false;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Demo Window", nullptr, &showDemoWindow_);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Transport control panel
    if (ImGui::Begin("Transport", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static float tempo = 120.0f;
        static float swing = 0.0f;
        static int timeSignatureIndex = 0;
        const char* timeSignatures[] = {"4/4", "3/4", "6/8"};

        bool tempoChanged = ImGui::SliderFloat("Tempo (BPM)", &tempo, 60.0f, 180.0f);
        bool swingChanged = ImGui::SliderFloat("Swing", &swing, 0.0f, 0.6f, "%.2f");
        bool timeSignatureChanged = ImGui::Combo("Time Signature", &timeSignatureIndex, timeSignatures, 3);

        ImGui::Spacing();
        if (ImGui::Button("Play", ImVec2(100, 0))) {
            if (sequencer_) {
                sequencer_->getTransport().play();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop", ImVec2(100, 0))) {
            if (sequencer_) {
                sequencer_->getTransport().stop();
            }
        }

        ImGui::Spacing();

        // Calculate current step from audio frame position (real-time playhead)
        if (audioEngine_ && sequencer_) {
            uint64_t frameCount = audioEngine_->getTotalFramesProcessed();
            uint32_t sampleRate = audioEngine_->getSampleRate();
            float currentTempo = sequencer_->getTransport().getTempo();
            
            // Calculate samples per step
            // At 120 BPM: 120 beats/min, quarter note per beat, 4 quarters per bar, 16 steps per bar
            // samples_per_beat = sampleRate * 60 / tempo
            // samples_per_step = samples_per_beat / 4 (4 steps per beat in 16-step grid)
            uint64_t samplesPerBeat = (sampleRate * 60) / static_cast<uint32_t>(currentTempo);
            uint64_t samplesPerStep = samplesPerBeat / 4;
            
            if (samplesPerStep > 0) {
                currentStep_ = (frameCount / samplesPerStep) % 16;
            }
        }

        ImGui::Text("Current Step: %u / 15", currentStep_);
        ImGui::Text("Frame Count: %llu", audioEngine_ ? audioEngine_->getTotalFramesProcessed() : 0);

        // Update Transport if UI values changed
        if (sequencer_) {
            if (tempoChanged) {
                sequencer_->getTransport().setTempo(tempo);
            }
            if (swingChanged) {
                sequencer_->getTransport().setSwing(swing);
            }
            if (timeSignatureChanged) {
                const char* tsValue = timeSignatures[timeSignatureIndex];
                sequencer_->getTransport().setTimeSignature(tsValue);
            }
        }

        ImGui::End();
    }

    // Save Pattern Dialog
    if (showSaveDialog_) {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Save Pattern", &showSaveDialog_, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Pattern name:");
            ImGui::InputText("##patternName", patternNameBuffer_, sizeof(patternNameBuffer_));

            ImGui::Spacing();
            ImGui::Separator();

            if (ImGui::Button("Save", ImVec2(120, 0))) {
                if (patternNameBuffer_[0] != '\0' && sequencer_) {
                    const Pattern& pattern = sequencer_->getPattern();
                    if (patternManager_->savePattern(patternNameBuffer_, pattern)) {
                        ImGui::OpenPopup("Save Success");
                    } else {
                        ImGui::OpenPopup("Save Error");
                    }
                    std::memset(patternNameBuffer_, 0, sizeof(patternNameBuffer_));
                    showSaveDialog_ = false;
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                std::memset(patternNameBuffer_, 0, sizeof(patternNameBuffer_));
                showSaveDialog_ = false;
            }

            ImGui::EndPopup();
        }
    }

    // Load Pattern Dialog
    if (showLoadDialog_) {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Load Pattern", &showLoadDialog_, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Available patterns:");
            ImGui::Separator();

            const auto availablePatterns = patternManager_->getAvailablePatterns();
            for (const auto& patternFile : availablePatterns) {
                if (ImGui::Button(patternFile.c_str(), ImVec2(300, 0))) {
                    if (sequencer_ && patternManager_->loadPattern(patternFile, sequencer_->getPattern())) {
                        ImGui::OpenPopup("Load Success");
                    } else {
                        ImGui::OpenPopup("Load Error");
                    }
                    showLoadDialog_ = false;
                }
            }

            if (availablePatterns.empty()) {
                ImGui::Text("No patterns found.");
            }

            ImGui::Spacing();
            ImGui::Separator();

            if (ImGui::Button("Close", ImVec2(300, 0))) {
                showLoadDialog_ = false;
            }

            ImGui::EndPopup();
        }
    }

    // Success/Error popups
    if (ImGui::BeginPopupModal("Save Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Pattern saved successfully!");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Load Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Pattern loaded successfully!");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Save Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to save pattern.");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Load Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to load pattern.");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Step Editor
    if (stepEditor_ && sequencer_) {
        stepEditor_->render(sequencer_, currentStep_);
    }

    // Sample Browser
    if (sampleBrowser_ && sequencer_ && audioEngine_) {
        // Note: SamplePlayer is owned by AudioEngine, would need getter
        // For now, just render the UI with nullptr for SamplePlayer
        sampleBrowser_->render(sequencer_, nullptr, currentStep_ % 8);
    }

    // Swing Visualizer
    if (swingVisualizer_ && sequencer_) {
        swingVisualizer_->render(sequencer_);
    }

    // Demo window
    if (showDemoWindow_) {
        ImGui::ShowDemoWindow(&showDemoWindow_);
    }

    // Rendering
    ImGui::Render();
}

void Window::renderFrame()
{
    // Render ImGui
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(sdlWindow_));
}

bool Window::processFrame()
{
    handleEvents();
    renderUI();
    renderFrame();

    return isOpen_;
}

} // namespace DrumMachine
