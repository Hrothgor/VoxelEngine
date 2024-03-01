#pragma once

#ifndef IMGUIPANEL_HPP_
#define IMGUIPANEL_HPP_

#include "Include.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class ImGuiPanel {
    public:
        ImGuiPanel() = default;
        ~ImGuiPanel() = default;

        virtual void Start() = 0;
        virtual void Stop() = 0;

        virtual void BeginFrame() = 0;
        virtual void ImGuiRender() = 0;
        virtual void EndFrame() = 0;
};

#endif /* !IMGUIPANEL_HPP_ */