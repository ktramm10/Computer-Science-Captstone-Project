#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager {
    private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, std::string> texturePaths;
    std::unordered_map<std::string, sf::Font> fonts;
    std::unordered_map<std::string, std::string> fontPaths;

    public:
    bool loadFont(const std::string& id, const std::string& filePath);
    bool loadFontManifest(const std::string& filePath);
    sf::Font& getFont(const std::string& id);
    const std::unordered_map<std::string, sf::Font>& getFonts() const {
        return fonts;
    }
    bool hasFont(const std::string& id) const;
    const std::unordered_map<std::string, std::string>& getFontPaths() const;
    void createDefaultFont();
    bool loadTexture(const std::string& id, const std::string& filePath);
    bool loadTextureManifest(const std::string& filePath);
    sf::Texture& getTexture(const std::string& id);
    const std::unordered_map<std::string, sf::Texture> getTextures() const {
        return textures;
    }
    bool hasTexture(const std::string& id) const;
    const std::unordered_map<std::string, std::string>& getTexturePaths() const;
    static constexpr const char* DefaultTextureID = "default";
    static constexpr const char* DefaultFontID = "default";
    void createDefaultTexture();
};