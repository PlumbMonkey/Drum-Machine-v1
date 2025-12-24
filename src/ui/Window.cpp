#include "Window.h"
#include "StepEditor.h"
#include "PatternManager.h"
#include "../audio/AudioEngine.h"
#include "../audio/MidiManager.h"
#include "../audio/SamplePlayer.h"
#include "../sequencer/Sequencer.h"
#include <SDL2/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <cstring>

// OpenGL functions
#ifdef _WIN32
extern "C" {
    void glClear(unsigned int);
    void glClearColor(float, float, float, float);
}
#define GL_COLOR_BUFFER_BIT 0x00004000
#endif

namespace DrumMachine {

Window::Window(uint32_t width, uint32_t height)
    : width_(width), height_(height), isOpen_(true),
      sdlWindow_(nullptr), glContext_(nullptr),
      audioEngine_(nullptr), sequencer_(nullptr), midiManager_(nullptr), samplePlayer_(nullptr),
      currentStep_(0)
{
    stepEditor_ = std::make_unique<StepEditor>();
    patternManager_ = std::make_unique<PatternManager>();
    std::memset(patternNameBuffer_, 0, sizeof(patternNameBuffer_));
}

Window::~Window()
{
    shutdown();
}

void Window::setSamplePlayer(SamplePlayer* samplePlayer)
{
    samplePlayer_ = samplePlayer;
    if (stepEditor_) {
        stepEditor_->setSamplePlayer(samplePlayer);
    }
}

void Window::setSamplePlayers(const std::array<SamplePlayer*, 8>& players)
{
    std::cout << "[WINDOW] setSamplePlayers called with " << players.size() << " players" << std::flush << std::endl;
    for (size_t i = 0; i < players.size(); ++i) {
        std::cout << "  Player[" << i << "]: " << (players[i] ? "VALID" : "nullptr") << std::flush << std::endl;
    }
    if (stepEditor_) {
        stepEditor_->setSamplePlayers(players);
        std::cout << "[WINDOW] Array passed to StepEditor" << std::flush << std::endl;
    }
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    glContext_ = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(sdlWindow_));
    if (!glContext_) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(sdlWindow_));
        SDL_Quit();
        return false;
    }

    SDL_GL_SetSwapInterval(1); // Enable vsync
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Setup ImGui backends
    ImGui_ImplSDL2_InitForOpenGL(reinterpret_cast<SDL_Window*>(sdlWindow_), glContext_);
    ImGui_ImplOpenGL3_Init("#version 150");

    std::cout << "Window initialized successfully" << std::endl;
    return true;
}

void Window::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (glContext_) {
        SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(glContext_));
        glContext_ = nullptr;
    }

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

    // Simple menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load Sample...", "Ctrl+O")) {
                // TODO: Implement file dialog for loading samples
                std::cout << "[UI] Load sample dialog not yet implemented" << std::endl;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Pattern...", "Ctrl+S")) {
                // TODO: Implement file dialog for saving patterns
                std::cout << "[UI] Save pattern dialog not yet implemented" << std::endl;
            }
            if (ImGui::MenuItem("Load Pattern...", "Ctrl+L")) {
                // TODO: Implement file dialog for loading patterns
                std::cout << "[UI] Load pattern dialog not yet implemented" << std::endl;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ESC")) {
                isOpen_ = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Transport window - tempo and swing controls
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, 140), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.95f);
    
    if (ImGui::Begin("Transport")) {
        static float tempo = 120.0f;
        static float swing = 0.0f;
        static float masterVolume = 1.0f;

        // Play/Stop buttons - control the Transport (sequencer playback)
        if (sequencer_) {
            if (sequencer_->getTransport().getPlayState() == Transport::PlayState::Playing) {
                if (ImGui::Button("Stop##audio", ImVec2(60, 0))) {
                    sequencer_->getTransport().stop();
                }
            } else {
                if (ImGui::Button("Play##audio", ImVec2(60, 0))) {
                    sequencer_->getTransport().play();
                }
            }
            ImGui::SameLine();
            ImGui::Text("%s", sequencer_->getTransport().getPlayState() == Transport::PlayState::Playing ? "Playing" : "Stopped");
        }

        ImGui::Separator();

        ImGui::SliderFloat("Tempo (BPM)", &tempo, 60.0f, 180.0f);
        ImGui::SliderFloat("Swing (%)", &swing, 0.0f, 0.6f, "%.2f");
        ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 1.5f, "%.2f");

        if (sequencer_) {
            sequencer_->getTransport().setTempo(tempo);
            sequencer_->getTransport().setSwing(swing);
        }

        // Calculate and display current step
        if (audioEngine_ && sequencer_) {
            // Only advance step if transport is playing
            if (sequencer_->getTransport().getPlayState() == Transport::PlayState::Playing) {
                uint64_t frameCount = audioEngine_->getTotalFramesProcessed();
                uint32_t sampleRate = audioEngine_->getSampleRate();
                float currentTempo = sequencer_->getTransport().getTempo();
                uint64_t samplesPerBeat = (sampleRate * 60) / static_cast<uint32_t>(currentTempo);
                uint64_t samplesPerStep = samplesPerBeat / 4;
                if (samplesPerStep > 0) {
                    currentStep_ = (frameCount / samplesPerStep) % 16;
                }
                ImGui::Text("Step: %u / 15", currentStep_);
            } else {
                ImGui::Text("Step: stopped");
            }
        }

        ImGui::End();
    }

    // Step Editor window - main grid (moved 20% right to show track names)
    ImGui::SetNextWindowPos(ImVec2(256, 180), ImGuiCond_FirstUseEver);  // 1280 * 0.2 = 256
    ImGui::SetNextWindowSize(ImVec2(1000, 480), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.95f);
    
    if (ImGui::Begin("Step Editor")) {
        if (stepEditor_ && sequencer_) {
            stepEditor_->render(sequencer_, currentStep_);
        }
        ImGui::End();
    }

    // End frame
    ImGui::Render();
}

void Window::renderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(sdlWindow_));
}

bool Window::processFrame()
{
    handleEvents();
    renderUI();
    renderFrame();

    // Frame rate limiting
    static uint32_t lastFrameTime = SDL_GetTicks();
    uint32_t currentTime = SDL_GetTicks();
    uint32_t frameDelta = currentTime - lastFrameTime;
    const uint32_t targetFrameTime = 16; // ~60 FPS
    
    if (frameDelta < targetFrameTime) {
        SDL_Delay(targetFrameTime - frameDelta);
    }
    lastFrameTime = SDL_GetTicks();

    return isOpen_;
}

} // namespace DrumMachine
