#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager {
    private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, std::string> texturePaths;

    public:
    bool loadTexture(const std::string& id, const std::string& filePath);
    sf::Texture& getTexture(const std::string& id);
    bool hasTexture(const std::string& id) const;
    const std::unordered_map<std::string, std::string>& getTexturePaths() const;
};