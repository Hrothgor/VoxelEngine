#pragma once

#ifndef IMGUILAYER_HPP_
#define IMGUILAYER_HPP_

#include "Include.hpp"
#include "ImGuiPanel.hpp"

class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void Start(GLFWwindow* window);
        void Stop();

        void ImGuiRenderDockSpace();

        void BeginFrame();
        void ImGuiRender();
        void EndFrame();

    protected:
    private:
        std::vector<std::unique_ptr<ImGuiPanel>> _Panels;
};

#endif /* !IMGUILAYER_HPP_ */