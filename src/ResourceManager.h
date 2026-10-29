#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

/**
 * @class ResourceManager
 * @brief Loads, stores, and retrieves textures, fonts, sound buffers, and music paths by resource ID.
 */
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
        /**
         * @brief Loads a font from a file and associates it with an ID.
         * @param id The ID to associate with the font.
         * @param filePath The path to the font file.
         * @return True if the font was loaded successfully, false otherwise.
         */
    bool loadFont(const std::string& id, const std::string& filePath);

        /**
         * @brief Loads font entries from a JSON manifest.
         * @param filePath The path to the font manifest file.
         * @return True if all fonts were loaded successfully, false otherwise.
         */
    bool loadFontManifest(const std::string& filePath);

        /**
         * @brief Retrieves a font by ID.
         * @param id The ID of the font to retrieve.
         * @return A reference to the requested font.
         */
    sf::Font& getFont(const std::string& id);

        /**
         * @brief Retrieves all loaded fonts.
         * @return A const reference to the loaded font map.
         */
    const std::unordered_map<std::string, sf::Font>& getFonts() const {
        return fonts;
    }

        /**
         * @brief Checks whether a font ID has been loaded.
         * @param id The ID of the font to check.
         * @return True if the font exists, false otherwise.
         */
    bool hasFont(const std::string& id) const;

        /**
         * @brief Retrieves the source file paths for loaded fonts.
         * @return A const reference to the font path map.
         */
    const std::unordered_map<std::string, std::string>& getFontPaths() const { return fontPaths; };

        /**
         * @brief Creates and stores the default font resource.
         */
    void createDefaultFont();

        /**
         * @brief Resource ID used for the default font.
         */
    static constexpr const char* DefaultFontID = "default";

        /**
         * @brief Loads a texture from a file and associates it with an ID.
         * @param id The ID to associate with the texture.
         * @param filePath The path to the texture file.
         * @return True if the texture was loaded successfully, false otherwise.
         */
    bool loadTexture(const std::string& id, const std::string& filePath);

        /**
         * @brief Loads texture entries from a JSON manifest.
         * @param filePath The path to the texture manifest file.
         * @return True if all textures were loaded successfully, false otherwise.
         */
    bool loadTextureManifest(const std::string& filePath);

        /**
         * @brief Retrieves a texture by ID.
         * @param id The ID of the texture to retrieve.
         * @return A reference to the requested texture.
         */
    sf::Texture& getTexture(const std::string& id);

        /**
         * @brief Retrieves all loaded textures.
         * @return A copy of the loaded texture map.
         */
    const std::unordered_map<std::string, sf::Texture> getTextures() const {
        return textures;
    }

        /**
         * @brief Checks whether a texture ID has been loaded.
         * @param id The ID of the texture to check.
         * @return True if the texture exists, false otherwise.
         */
    bool hasTexture(const std::string& id) const;

        /**
         * @brief Retrieves the source file paths for loaded textures.
         * @return A const reference to the texture path map.
         */
    const std::unordered_map<std::string, std::string>& getTexturePaths() const { return texturePaths; };

        /**
         * @brief Resource ID used for the default texture.
         */
    static constexpr const char* DefaultTextureID = "default";

        /**
         * @brief Creates and stores the default texture resource.
         */
    void createDefaultTexture();

        /**
         * @brief Loads a sound buffer from a file and associates it with an ID.
         * @param id The ID to associate with the sound buffer.
         * @param filePath The path to the sound buffer file.
         * @return True if the sound buffer was loaded successfully, false otherwise.
         */
    bool loadSoundBuffer(const std::string& id, const std::string& filePath);

        /**
         * @brief Retrieves a sound buffer by ID.
         * @param id The ID of the sound buffer to retrieve.
         * @return A reference to the requested sound buffer.
         */
    sf::SoundBuffer& getSoundBuffer(const std::string& id);

        /**
         * @brief Checks whether a sound buffer ID has been loaded.
         * @param id The ID of the sound buffer to check.
         * @return True if the sound buffer exists, false otherwise.
         */
    bool hasSoundBuffer(const std::string& id) const;

        /**
         * @brief Loads sound buffer entries from a JSON manifest.
         * @param filePath The path to the sound buffer manifest file.
         * @return True if all sound buffers were loaded successfully, false otherwise.
         */
    bool loadSoundBufferManifest(const std::string& filePath);

        /**
         * @brief Retrieves all loaded sound buffers.
         * @return A const reference to the loaded sound buffer map.
         */
    const std::unordered_map<std::string, sf::SoundBuffer>& getSoundBuffers() const {
        return soundBuffers;
    }

        /**
         * @brief Retrieves the source file paths for loaded sound buffers.
         * @return A const reference to the sound buffer path map.
         */
    const std::unordered_map<std::string, std::string>& getSoundBufferPaths() const { return soundBufferPaths; }

        /**
         * @brief Registers a music file path under a resource ID.
         * @param id The ID to associate with the music file.
         * @param filePath The path to the music file.
         * @return True if the music file path was registered successfully, false otherwise.
         */
    bool loadMusic(const std::string& id, const std::string& filePath);

        /**
         * @brief Checks whether a music ID has been registered.
         * @param id The ID of the music entry to check.
         * @return True if the music entry exists, false otherwise.
         */
    bool hasMusic(const std::string& id) const;

        /**
         * @brief Loads music entries from a JSON manifest.
         * @param filePath The path to the music manifest file.
         * @return True if all music entries were loaded successfully, false otherwise.
         */
    bool loadMusicManifest(const std::string& filePath);

        /**
         * @brief Retrieves registered music file paths.
         * @return A const reference to the music path map.
         */
    const std::unordered_map<std::string, std::string>& getMusicPaths() const { return musicPaths; };
};