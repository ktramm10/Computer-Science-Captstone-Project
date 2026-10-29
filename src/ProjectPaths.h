#pragma once

#include <filesystem>

#ifndef PROJECT_ROOT_PATH
#define PROJECT_ROOT_PATH "."
#endif

/**
 * @brief Gets the root path of the project
 * @return Returns a file path object representing the root path.
 */
inline std::filesystem::path getProjectRootPath()
{
    return std::filesystem::path(PROJECT_ROOT_PATH).lexically_normal();
}
/**
 * @brief Gets the relative path of the project by normalizing and iterating over the path and removing any double or single dots referencing parent directories.
 * @param path The path you would like to make relative.
 * @return Returns the relative file path.
 */
inline std::filesystem::path getProjectRelativePath(const std::filesystem::path& path)
{
    const auto normalizedPath = path.lexically_normal();
    std::filesystem::path relativePath;

    for (const auto& part : normalizedPath) {
        const auto partString = part.string();
        if (relativePath.empty() && (partString == "." || partString == "..")) {
            continue;
        }
        relativePath /= part;
    }

    return relativePath.empty() ? normalizedPath : relativePath;
}
/**
 * @brief Takes a path as an argument and turns it into a path that points to the correct location within the project.
 * @param path The path you would like to resolve within the project.
 * @return Returns the path you passed in modified to point to the correct location within the project.
 */
inline std::filesystem::path resolveProjectPath(const std::filesystem::path& path)
{
    if (path.empty() || path.is_absolute()) {
        return path.lexically_normal();
    }

    return (getProjectRootPath() / getProjectRelativePath(path)).lexically_normal();
}