#include "ViewportPanel.hpp"
#include "Renderer/Renderer.hpp"

ViewportPanel::ViewportPanel() {
}

ViewportPanel::~ViewportPanel() {
}

void ViewportPanel::ImGuiRender() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0, 0});
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);

    bool isFocus = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)(size_t)Renderer::Get()->GetScreenTexture(), size, ImVec2 {0, 1}, ImVec2 {1, 0});

    ImGui::End();
    ImGui::PopStyleVar();
}