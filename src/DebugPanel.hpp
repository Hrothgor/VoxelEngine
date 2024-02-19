#pragma once

#ifndef DEBUGPANEL_HPP_
#define DEBUGPANEL_HPP_

#include "Include.hpp"
#include "ImGuiPanel.hpp"

class DebugPanel : public ImGuiPanel {
    public:
        DebugPanel();
        ~DebugPanel();

        virtual void Start() override {};
        virtual void End() override {};

        virtual void BeginFrame() override {};
        virtual void ImGuiRender() override;
        virtual void EndFrame() override {};
};

#endif /* !DEBUGPANEL_HPP_ */