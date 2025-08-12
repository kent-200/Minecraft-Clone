#pragma once

#include "header.h"

class ImGuiWrapper {
private:
    GLFWwindow* window;

public:
    ImGuiWrapper();
    ~ImGuiWrapper();
    
    // Initialize ImGui with the given window
    void init(GLFWwindow* window);
    
    // Begin new frame
    void newFrame();
    
    // Render camera and performance UI
    void renderUI(const glm::vec3& position, float yaw, float pitch, float averageFps);
    
    // Render ImGui
    void render();
    
    // Shutdown ImGui
    void shutdown();
};




#include "imguiWrapper.h"

ImGuiWrapper::ImGuiWrapper() : window(nullptr) {}

ImGuiWrapper::~ImGuiWrapper() {
    shutdown();
}

void ImGuiWrapper::init(GLFWwindow* win) {
    window = win;
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); 
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    #ifdef __APPLE__
    ImGui_ImplOpenGL3_Init("#version 150");
    #else
    ImGui_ImplOpenGL3_Init("#version 130");
    #endif
}

void ImGuiWrapper::newFrame() {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiWrapper::renderUI(const glm::vec3& position, float yaw, float pitch, float averageFps) {
    // Set ImGui window position and size
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 80), ImGuiCond_Always);

    // Camera and Performance UI
    ImGui::Begin("Camera and Performance", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
    ImGui::Text("Yaw = %.2f, Pitch = %.2f", yaw, pitch);
    ImGui::Text("FPS: %.1f (%.1f)", averageFps, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ImGuiWrapper::render() {
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiWrapper::shutdown() {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

