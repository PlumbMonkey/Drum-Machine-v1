#include "Window.h"
#include "../audio/AudioEngine.h"
#include "../sequencer/Sequencer.h"
#include <SDL2/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

namespace DrumMachine {

Window::Window(uint32_t width, uint32_t height)
    : width_(width), height_(height), isOpen_(true),
      sdlWindow_(nullptr), glContext_(nullptr),
      audioEngine_(nullptr), sequencer_(nullptr),
      showDemoWindow_(false)
{
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
        static int timeSignatureIndex = 0;
        const char* timeSignatures[] = {"4/4", "3/4", "6/8"};

        ImGui::SliderFloat("Tempo (BPM)", &tempo, 60.0f, 180.0f);
        ImGui::Combo("Time Signature", &timeSignatureIndex, timeSignatures, 3);

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

        // Update sequencer with UI values
        if (sequencer_) {
            sequencer_->getTransport().setTempo(tempo);
            const char* tsValue = timeSignatures[timeSignatureIndex];
            sequencer_->getTransport().setTimeSignature(tsValue);
        }

        ImGui::End();
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
