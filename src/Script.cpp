#include "Script.h"
#include <iostream>

/**
 * @brief Loads the list of available script JSON files from a folder.
 * @param folder The folder to search for script files.
 * @return A vector containing the paths of all JSON script files found in the folder.
 */
std::vector<std::filesystem::path> Script::loadScriptsList(const std::filesystem::path &folder)
{
    std::vector<std::filesystem::path> scriptPaths;
    if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) {
        std::cerr << "Scene forlder does not exist: " << folder << std::endl;
        return scriptPaths;
    }

    for (const auto& file : std::filesystem::directory_iterator(folder)) {
        if (!file.is_regular_file()) continue;
        auto path = file.path();
        if (path.extension() == ".json") {
            scriptPaths.push_back(path);
        }
    }
    return scriptPaths;
}
