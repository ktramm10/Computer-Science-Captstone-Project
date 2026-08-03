#include "ResourceManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

sf::Texture& ResourceManager::getTexture(const std::string& id){
    auto it = textures.find(id);
    if (it == textures.end())
    {
        throw std::runtime_error("Texture not found: " + id);
    }
    return it->second;
}

bool ResourceManager::hasTexture(const std::string& id) const{
    auto it = textures.find(id);
    return (it != textures.end());
}

const std::unordered_map<std::string, std::string> &ResourceManager::getTexturePaths() const
{
    return texturePaths;
}

void ResourceManager::createDefaultTexture()
{
    if (hasTexture(DefaultTextureID)) {
        return;
    }

    sf::Texture texture;
    if (!texture.loadFromFile("C:\\Users\\battl\\OneDrive\\Documents\\GitHub\\Computer-Science-Captstone-Project\\src\\images\\placeholderSprite 32x32..png")){
        throw std::runtime_error("Failed to create default texture.");
    }

    textures.emplace(DefaultTextureID, std::move(texture));
    texturePaths[DefaultTextureID] = "";
}

bool ResourceManager::loadFont(const std::string &id, const std::string &filePath)
{
    if (hasFont(id)) {
        fontPaths[id] = filePath;
        return true;
    }
    sf::Font font;
    if (!font.openFromFile(filePath)) {
        return false;
    }
    fonts.emplace(id, std::move(font));
    fontPaths[id] = filePath;
    return true;

}

bool ResourceManager::loadFontManifest(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open font manifest: " << filePath << std::endl;
        return false;
    }

    nlohmann::json manifestData;
    try {
        file >> manifestData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse font manigest: " << e.what() << std::endl;
        return false;
    }

    if (!manifestData.contains("fonts") || !manifestData["fonts"].is_array()) {
        std::cerr << "Font manifest has no fonts array." << std::endl;
        return false;
    }
    
    bool loadedAllFonts = true;
    for (const auto& fontData : manifestData["fonts"]) {
        std::string id = fontData.value("id", "");
        std::string path = fontData.value("path", "");

        if (id.empty() || path.empty()) {
            std::cerr << "Invalid font manifest entry." << std::endl;
            loadedAllFonts = false;
            continue;
        }

        if (!loadFont(id, path)) {
            std::cerr << "Failed to load Font: " << id << " from " << path << std::endl;
            loadedAllFonts = false;
        }
    }
    return loadedAllFonts;
}

sf::Font& ResourceManager::getFont(const std::string &id)
{
    auto it = fonts.find(id);
    if (it == fonts.end())
    {
        throw std::runtime_error("Font not found: " + id);
    }
    return it->second;
}

bool ResourceManager::hasFont(const std::string &id) const
{
    auto it = fonts.find(id);
    return (it != fonts.end());
}

const std::unordered_map<std::string, std::string> &ResourceManager::getFontPaths() const
{
    return fontPaths;
}

void ResourceManager::createDefaultFont()
{
    if (hasFont(DefaultFontID)) {
        return;
    }

    sf::Font font;
    if (!font.openFromFile("C:/Users/battl/OneDrive/Documents/GitHub/Computer-Science-Captstone-Project/src/fonts/Times New Roman.ttf")) {
        throw std::runtime_error("Failed to create default font.");
    }
    fonts.emplace(DefaultFontID, std::move(font));
    fontPaths[DefaultFontID] = "";
}

bool ResourceManager::loadTexture(const std::string &id, const std::string &filePath)
{
    if (hasTexture(id)) {
        texturePaths[id] = filePath;
        return true;
    }
    sf::Texture texture;
    if (!texture.loadFromFile(filePath)) return false;
    textures.emplace(id, std::move(texture));
    texturePaths[id] = filePath;
    return true;
}

bool ResourceManager::loadTextureManifest(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open texture manifest: " << filePath << std::endl;
        return false;
    }

    nlohmann::json manifestData;
    try {
        file >> manifestData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse texture manifest: " << e.what() << std::endl;
        return false;
    }

    if (!manifestData.contains("textures") || !manifestData["textures"].is_array()) {
        std::cerr << "Texture manifest has no textures array." << std::endl;
        return false;
    }

    bool loadedAllTextures = true;
    for (const auto& textureData : manifestData["textures"]) {
        std::string id = textureData.value("id", "");
        std::string path = textureData.value("path", "");

        if (id.empty() || path.empty()) {
            std::cerr << "Invalid texture manifest entry." << std::endl;
            loadedAllTextures = false;
            continue;
        }

        if (!loadTexture(id, path)) {
            std::cerr << "Failed to load texture: " << id << " from " << path << std::endl;
            loadedAllTextures = false;
        }
    }

    return loadedAllTextures;
}