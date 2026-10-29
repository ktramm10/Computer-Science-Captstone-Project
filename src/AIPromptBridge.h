#pragma once

#include <string>
#include <utility>
#include <vector>
/**
 * @struct AIPromptResult
 * @brief Structure to hold the results of an AI prompt prediction.
 * @details This structure contains various fields to store the results of an AI prompt prediction,
 *  including the original prompt, genre, scene template, entities, components, diagnostics report,
 *  label distribution, requirement coverage, accuracy kind, training timestamp, dataset row count,
 *  missing cells after cleaning, model accuracy, and confidence level.
 */
struct AIPromptResult {
    std::string prompt;
    std::string genre;
    std::string sceneTemplate;
    std::vector<std::string> entities;
    std::vector<std::string> components;
    std::string diagnosticsReport;
    std::vector<std::pair<std::string, int>> labelDistribution;
    std::vector<std::pair<std::string, bool>> requirementCoverage;
    std::string accuracyKind;
    std::string trainedAtUtc;
    int datasetRowCount = 0;
    int missingCellsAfterCleaning = 0;
    float modelAccuracy = 0.0f;
    float confidence = 0.0f;
};

class AIPromptBridge {
    public:
    bool predict(const std::string& prompt, AIPromptResult& result);
    const std::string& getLastError() const { return lastError; }

    private:
    std::string lastError;
};