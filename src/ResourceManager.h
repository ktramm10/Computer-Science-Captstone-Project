#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager {
    private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, std::string> texturePaths;
    std::unordered_map<std::string, sf::Font> fonts;
    std::unordered_map<std::string, std::string> fontPaths;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, std::string> soundBufferPaths;
    std::unordered_map<std::string, std::string> musicPaths;
    

    public:
    bool loadFont(const std::string& id, const std::string& filePath);
    bool loadFontManifest(const std::string& filePath);
    sf::Font& getFont(const std::string& id);
    const std::unordered_map<std::string, sf::Font>& getFonts() const {
        return fonts;
    }
    bool hasFont(const std::string& id) const;
    const std::unordered_map<std::string, std::string>& getFontPaths() const { return fontPaths; };
    void createDefaultFont();
    static constexpr const char* DefaultFontID = "default";

    bool loadTexture(const std::string& id, const std::string& filePath);
    bool loadTextureManifest(const std::string& filePath);
    sf::Texture& getTexture(const std::string& id);
    const std::unordered_map<std::string, sf::Texture> getTextures() const {
        return textures;
    }
    bool hasTexture(const std::string& id) const;
    const std::unordered_map<std::string, std::string>& getTexturePaths() const { return texturePaths; };
    static constexpr const char* DefaultTextureID = "default";
    void createDefaultTexture();

    bool loadSoundBuffer(const std::string& id, const std::string& filePath);
    sf::SoundBuffer& getSoundBuffer(const std::string& id);
    bool hasSoundBuffer(const std::string& id) const;
    bool loadSoundBufferManifest(const std::string& filePath);
    const std::unordered_map<std::string, sf::SoundBuffer>& getSoundBuffers() const {
        return soundBuffers;
    }
    const std::unordered_map<std::string, std::string>& getSoundBufferPaths() const { return soundBufferPaths; }

    bool loadMusic(const std::string& id, const std::string& filePath);
    bool hasMusic(const std::string& id) const;
    bool loadMusicManifest(const std::string& filePath);
    const std::unordered_map<std::string, std::string>& getMusicPaths() const { return musicPaths; };
};