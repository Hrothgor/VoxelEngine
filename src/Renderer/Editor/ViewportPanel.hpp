#pragma once

#ifndef VIEWPORTPANEL_HPP_
#define VIEWPORTPANEL_HPP_

#include "Include.hpp"
#include "ImGuiPanel.hpp"

class ViewportPanel : public ImGuiPanel {
    public:
        ViewportPanel();
        ~ViewportPanel();

        virtual void Start() override {};
        virtual void Stop() override {};

        virtual void BeginFrame() override {};
        virtual void ImGuiRender() override;
        virtual void EndFrame() override {};
};

#endif /* !VIEWPORTPANEL_HPP_ */