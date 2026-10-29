#include "AIPromptBridge.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

/**
 * @brief Anonymous namespace for internal helper functions.
 * 
 * @details This namespace contains various helper functions used internally by the AIPromptBridge class. These functions handle tasks such as opening and closing pipes, finding the project root, quoting command-line arguments, running commands, and parsing JSON data.
 */
namespace {

#if defined(_WIN32)
/**
 * @brief Opens a pipe to execute a command and read its output.
 * @param command The command to execute.
 * @return A pointer to the opened pipe.
 */
FILE* openPipe(const std::string& command)
{
    const std::string wrappedCommand = "\"" + command + "\" 2>&1";
    return _popen(wrappedCommand.c_str(), "r");
}
/**
 * @brief Closes a previously opened pipe.
 */
int closePipe(FILE* pipe)
{
    return _pclose(pipe);
}
#else
FILE* openPipe(const std::string& command)
{
    return popen(command.c_str(), "r");
}

int closePipe(FILE* pipe)
{
    return pclose(pipe);
}
#endif
/**
 * @brief Finds the root directory of the project.
 * @return The path to the project root.
 */
std::filesystem::path findProjectRoot()
{
    auto path = std::filesystem::current_path();
    while (!path.empty()) {
        if (std::filesystem::exists(path / "src" / "AI" / "train_model.py")) {
            return path;
        }
        auto parentPath = path.parent_path();
        if (parentPath == path) {
            break;
        }
        path = parentPath;
    }
    return std::filesystem::current_path();
}
/**
 * @brief Quotes a command-line argument to handle spaces and special characters.
 * @param path The path to quote.
 * @return The quoted path.
 */
std::string quoteArgument(const std::filesystem::path& path)
{
    std::string value = std::filesystem::absolute(path).string();
    std::string quoted = "\"";
    for (char character : value) {
        if (character == '"') {
            quoted += "\\\"";
        }
        else {
            quoted += character;
        }
    }
    quoted += "\"";
    return quoted;
}
/**
 * @brief Quotes an executable path if it is absolute or has a parent path.
 * @param executable The executable path to quote.
 * @return The quoted executable path.
 */
std::string quoteExecutable(const std::filesystem::path& executable)
{
    if (executable.is_absolute() || executable.has_parent_path()) {
        return quoteArgument(executable);
    }
    return executable.string();
}
/**
 * @brief Finds the Python executable to use for running the AI predictor script.
 * @param projectRoot The root directory of the project.
 * @return The path to the Python executable.
 */
std::filesystem::path findPythonExecutable(const std::filesystem::path& projectRoot)
{
    auto venvPython = projectRoot / ".venv" / "Scripts" / "python.exe";
    if (std::filesystem::exists(venvPython)) {
        return venvPython;
    }
    return "py";
}
/**
 * @brief Creates a temporary file path for storing the AI prompt.
 */
std::filesystem::path makePromptFilePath()
{
    auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() / ("game_engine_prompt_" + std::to_string(timestamp) + ".txt");
}
/**
 * @brief Runs a command in the system shell and captures its output.
 * @param command The command to execute.
 * @return The output of the command as a string.
 */
std::string runCommand(const std::string& command)
{
    std::array<char, 256> buffer{};
    std::ostringstream output;
    FILE* pipe = openPipe(command);
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output << buffer.data();
    }
    closePipe(pipe);
    return output.str();
}
/**
 * @brief Reads an array of strings from a JSON object for a given key.
 * @param data The JSON object to read from.
 * @param key The key corresponding to the array of strings.
 * @return A vector of strings read from the JSON array.
 */
std::vector<std::string> readStringArray(const nlohmann::json& data, const char* key)
{
    std::vector<std::string> values;
    if (!data.contains(key) || !data[key].is_array()) {
        return values;
    }
    for (const auto& value : data[key]) {
        if (value.is_string()) {
            values.push_back(value.get<std::string>());
        }
    }
    return values;
}
/**
 * @brief Reads a map of string keys to integer values from a JSON object.
 * @param data The JSON object to read from.
 * @return A vector of pairs representing the string-integer map.
 */
std::vector<std::pair<std::string, int>> readIntMap(const nlohmann::json& data)
{
    std::vector<std::pair<std::string, int>> values;
    if (!data.is_object()) {
        return values;
    }

    for (auto iterator = data.begin(); iterator != data.end(); ++iterator) {
        if (iterator.value().is_number_integer()) {
            values.push_back({ iterator.key(), iterator.value().get<int>() });
        }
    }
    return values;
}
/**
 * @brief Reads the requirement coverage from a JSON report and returns it as a vector of pairs.
 * @param report The JSON object containing the requirement report.
 * @return A vector of pairs where each pair consists of a requirement name and a boolean indicating
 */
std::vector<std::pair<std::string, bool>> readRequirementCoverage(const nlohmann::json& report)
{
    const std::array<std::pair<const char*, const char*>, 11> requirements = {{
        { "Descriptive", "descriptive_method" },
        { "Predictive", "nondescriptive_method" },
        { "Dataset", "collected_or_available_datasets" },
        { "Decision", "decision_support_functionality" },
        { "Wrangling", "dataset_featurizing_parsing_cleaning_wrangling" },
        { "Prep", "data_exploration_and_preparation_methods" },
        { "Visuals", "data_visualization_functionalities" },
        { "Queries", "interactive_queries" },
        { "ML", "machine_learning_methods_and_algorithms" },
        { "Accuracy", "accuracy_evaluation" },
        { "Security", "industry_appropriate_security_features" },
    }};

    std::vector<std::pair<std::string, bool>> coverage;
    for (const auto& requirement : requirements) {
        coverage.push_back({ requirement.first, report.contains(requirement.second) && !report[requirement.second].empty() });
    }
    coverage.push_back({ "Monitoring", report.contains("monitor_and_maintain_tools") && !report["monitor_and_maintain_tools"].empty() });
    return coverage;
}
/**
 * @brief Formats the requirement report from the JSON data into a human-readable string.
 * @param data The JSON object containing the requirement report.
 * @return A formatted string representing the requirement report.
 */
std::string formatRequirementReport(const nlohmann::json& data)
{
    if (!data.contains("requirement_report") || !data["requirement_report"].is_object()) {
        return "No AI requirement report was returned by the predictor.";
    }

    std::ostringstream report;
    report << "AI Algorithm Diagnostics and Requirements:\n";
    report << data["requirement_report"].dump(2);
    return report.str();
}

}
/**
 * @brief Predicts the AI behavior based on the given prompt and fills the result structure.
 * @param prompt The input prompt for the AI predictor.
 * @param result The structure to hold the prediction results.
 * @return True if the prediction was successful, false otherwise.
 */
bool AIPromptBridge::predict(const std::string& prompt, AIPromptResult& result)
{
    lastError.clear();
    result = AIPromptResult{};

    if (prompt.empty()) {
        lastError = "Prompt is empty.";
        return false;
    }

    const auto projectRoot = findProjectRoot();
    const auto scriptPath = projectRoot / "src" / "AI" / "train_model.py";
    if (!std::filesystem::exists(scriptPath)) {
        lastError = "Could not find src/AI/train_model.py.";
        return false;
    }

    const auto promptFilePath = makePromptFilePath();
    {
        std::ofstream promptFile(promptFilePath);
        if (!promptFile.is_open()) {
            lastError = "Could not write prompt temp file.";
            return false;
        }
        promptFile << prompt;
    }

    const auto pythonExecutable = findPythonExecutable(projectRoot);
    const std::string command = quoteExecutable(pythonExecutable) + " " + quoteArgument(scriptPath) + " --predict-file " + quoteArgument(promptFilePath);
    const std::string output = runCommand(command);
    std::filesystem::remove(promptFilePath);

    if (output.empty()) {
        lastError = "Predictor did not return output.";
        return false;
    }

    try {
        const auto data = nlohmann::json::parse(output);
        result.prompt = data.value("prompt", prompt);
        result.genre = data.value("genre", "");
        result.sceneTemplate = data.value("scene_template", "");
        result.entities = readStringArray(data, "entities");
        result.components = readStringArray(data, "components");
        result.diagnosticsReport = formatRequirementReport(data);
        result.confidence = data.value("confidence", 0.0f);
        if (data.contains("requirement_report") && data["requirement_report"].is_object()) {
            const auto& report = data["requirement_report"];
            result.requirementCoverage = readRequirementCoverage(report);
            if (report.contains("accuracy_evaluation") && report["accuracy_evaluation"].is_object()) {
                result.modelAccuracy = report["accuracy_evaluation"].value("score", 0.0f);
                result.accuracyKind = report["accuracy_evaluation"].value("kind", "");
            }
            if (report.contains("monitor_and_maintain_tools") && report["monitor_and_maintain_tools"].is_object()) {
                const auto& monitoring = report["monitor_and_maintain_tools"];
                result.datasetRowCount = monitoring.value("dataset_row_count", 0);
                result.missingCellsAfterCleaning = monitoring.value("missing_cells_after_cleaning", 0);
                result.trainedAtUtc = monitoring.value("trained_at_utc", "");
                if (monitoring.contains("label_distribution")) {
                    result.labelDistribution = readIntMap(monitoring["label_distribution"]);
                }
            }
        }
    }
    catch (const std::exception& error) {
        lastError = std::string("Failed to parse predictor JSON: ") + error.what() + ". Output: " + output.substr(0, 300);
        return false;
    }

    return true;
}