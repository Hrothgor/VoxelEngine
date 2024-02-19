#pragma once

#ifndef IMGUIPANEL_HPP_
#define IMGUIPANEL_HPP_

#include "Include.hpp"

class ImGuiPanel {
    public:
        ImGuiPanel() = default;
        ~ImGuiPanel() = default;

        virtual void Start() = 0;
        virtual void End() = 0;

        virtual void BeginFrame() = 0;
        virtual void ImGuiRender() = 0;
        virtual void EndFrame() = 0;
};

#endif /* !IMGUIPANEL_HPP_ */