#include "LogPanel.hpp"
#include "Logger.hpp"

LogPanel::LogPanel() {
}

LogPanel::~LogPanel() {
}

void LogPanel::ImGuiRender() {
    ImGui::Begin("Logs");

    for (auto& log : Logger::Get()->GetLogs()) {
        ImVec4 color;
        switch (log.type) {
            case Logger::LogType::WARNING:
                color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                break;
            case Logger::LogType::ERROR:
                color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                break;
            case Logger::LogType::INFO:
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                break;
            case Logger::LogType::SUCCESS:
                color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
                break;
        }
        ImGui::TextColored(color, Logger::Get()->FormatLog(log).c_str());
    }
    ImGui::SetScrollHereY(1.0f);

    ImGui::End();
}