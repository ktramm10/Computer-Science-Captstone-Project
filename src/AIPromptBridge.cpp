#include "AIPromptBridge.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace {

#if defined(_WIN32)
FILE* openPipe(const std::string& command)
{
    const std::string wrappedCommand = "\"" + command + "\" 2>&1";
    return _popen(wrappedCommand.c_str(), "r");
}

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

std::string quoteExecutable(const std::filesystem::path& executable)
{
    if (executable.is_absolute() || executable.has_parent_path()) {
        return quoteArgument(executable);
    }
    return executable.string();
}

std::filesystem::path findPythonExecutable(const std::filesystem::path& projectRoot)
{
    auto venvPython = projectRoot / ".venv" / "Scripts" / "python.exe";
    if (std::filesystem::exists(venvPython)) {
        return venvPython;
    }
    return "py";
}

std::filesystem::path makePromptFilePath()
{
    auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() / ("game_engine_prompt_" + std::to_string(timestamp) + ".txt");
}

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

}

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
        result.confidence = data.value("confidence", 0.0f);
    }
    catch (const std::exception& error) {
        lastError = std::string("Failed to parse predictor JSON: ") + error.what() + ". Output: " + output.substr(0, 300);
        return false;
    }

    return true;
}