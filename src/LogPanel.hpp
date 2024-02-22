#pragma once

#ifndef LOGPANEL_HPP_
#define LOGPANEL_HPP_

#include "Include.hpp"
#include "ImGuiPanel.hpp"

class LogPanel : public ImGuiPanel {
    public:
        LogPanel();
        ~LogPanel();

        virtual void Start() override {};
        virtual void End() override {};

        virtual void BeginFrame() override {};
        virtual void ImGuiRender() override;
        virtual void EndFrame() override {};
};

#endif /* !LOGPANEL_HPP_ */