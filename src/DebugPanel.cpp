#include "DebugPanel.hpp"
#include "Engine.hpp"

DebugPanel::DebugPanel() {
}

DebugPanel::~DebugPanel() {
}

void DebugPanel::ImGuiRender() {
    ImGui::Begin("Debug");

    // Draw Text using engine framerate
    ImGui::Text("Framerate: %d FPS", Engine::Get()->GetFPS());
    ImGui::Text("Frame Time: %.3f ms", Engine::Get()->GetDeltaTime() * 1000);

    ImGui::End();
}