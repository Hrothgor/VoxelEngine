#include "DebugPanel.hpp"
#include "Engine.hpp"
#include "SimpleRenderer.hpp"

DebugPanel::DebugPanel() {
}

DebugPanel::~DebugPanel() {
}

void DebugPanel::ImGuiRender() {
    ImGui::Begin("Debug");

    // Draw Text using engine framerate
    ImGui::Text("Framerate: %d FPS", Engine::Get()->GetFPS());
    ImGui::Text("Frame Time: %.3f ms", Engine::Get()->GetDeltaTime() * 1000);

    // Reload Shader
    if (ImGui::Button("Reload Shader")) {
        SimpleRenderer::Get()->_Shader.Reload();
    }

    ImGui::End();
}