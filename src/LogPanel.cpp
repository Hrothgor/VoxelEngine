#include "LogPanel.hpp"
#include "Logger.hpp"

LogPanel::LogPanel() {
}

LogPanel::~LogPanel() {
}

void LogPanel::ImGuiRender() {
    ImGui::Begin("Logs");

    // Print last 100 logs
    std::vector<Logger::LogStruct> logs = Logger::Get()->GetLogs();
    for (int i = logs.size() >= 100 ? logs.size() - 100 : 0; i < logs.size(); i++) {
        auto log = logs[i];
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

    ImGui::End();
}