#include "ResourceManager.h"

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

bool ResourceManager::loadTexture(const std::string& id, const std::string& filePath) {
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