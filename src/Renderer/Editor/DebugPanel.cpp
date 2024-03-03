#include "DebugPanel.hpp"
#include "Engine.hpp"
#include "Profiler.hpp"
#include "Entities/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Shader/GeometryShader/GeometryShader.hpp"
#include "Renderer/Shader/ScreenShader/ScreenShader.hpp"

DebugPanel::DebugPanel() {
}

DebugPanel::~DebugPanel() {
}

void DebugPanel::ImGuiRender() {
    ImGui::Begin("Debug");

    // Draw Text using engine framerate
    ImGui::Text("Framerate: %d FPS", Engine::Get()->GetFPS());
    ImGui::Text("Frame Time: %.3f ms", Engine::Get()->GetDeltaTime() * 1000.0f);

    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "GPU frame: %.3f ms", Profiler::Get()->GetFrameTime("GPU Frame") * 1000.0f);
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "\tGeoBuffer: %.3f ms", Profiler::Get()->GetFrameTime("GPU::GBuffer") * 1000.0f);
    ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "\tScreenBuffer: %.3f ms", Profiler::Get()->GetFrameTime("GPU::ScreenBuffer") * 1000.0f);

    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "CPU frame: %.3f ms", Profiler::Get()->GetFrameTime("CPU Frame") * 1000.0f);

    ImGui::Separator();

    // Reload Shader
    if (ImGui::Button("Reload Geometry Shader")) {
        Renderer::Get()->GetGeometryShader().Reload();
    }
    if (ImGui::Button("Reload Screen Shader")) {
        Renderer::Get()->GetScreenShader().Reload();
    }

    ImGui::Separator();

    // Draw Text using engine camera position
    ImGui::Text("Camera Position: %.2f, %.2f, %.2f", Engine::Get()->GetMainCamera()->GetPosition().x, Engine::Get()->GetMainCamera()->GetPosition().y, Engine::Get()->GetMainCamera()->GetPosition().z);
    ImGui::Text("Camera Rotation: %.2f, %.2f, %.2f", Engine::Get()->GetMainCamera()->GetRotation().x, Engine::Get()->GetMainCamera()->GetRotation().y, Engine::Get()->GetMainCamera()->GetRotation().z);

    ImGui::End();
}