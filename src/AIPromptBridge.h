#pragma once

#include <string>
#include <vector>

struct AIPromptResult {
    std::string prompt;
    std::string genre;
    std::string sceneTemplate;
    std::vector<std::string> entities;
    std::vector<std::string> components;
    float confidence = 0.0f;
};

class AIPromptBridge {
    public:
    bool predict(const std::string& prompt, AIPromptResult& result);
    const std::string& getLastError() const { return lastError; }

    private:
    std::string lastError;
};