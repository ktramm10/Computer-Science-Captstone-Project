#include "ResourceManager.h"
#include "ProjectPaths.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

/**
 * @brief Returns a texture object associated with the given ID.
 * @param id The ID of the texture to retrieve.
 * @return A reference to the sf::Texture object associated with the given ID.
 * @throws std::runtime_error if the texture with the specified ID is not found.
 */
sf::Texture& ResourceManager::getTexture(const std::string& id){
    auto it = textures.find(id);
    if (it == textures.end())
    {
        throw std::runtime_error("Texture not found: " + id);
    }
    return it->second;
}
/**
 * @brief Checks if a texture with the given ID exists in the resource manager.
 * @param id The ID of the texture to check.
 * @return true if the texture exists, false otherwise.
 */
bool ResourceManager::hasTexture(const std::string& id) const{
    auto it = textures.find(id);
    return (it != textures.end());
}
/**
 * @brief Creates a default texture and adds it to the resource manager.
 * @throws std::runtime_error if the default texture cannot be created.
 */
void ResourceManager::createDefaultTexture()
{
    if (hasTexture(DefaultTextureID)) {
        return;
    }

    sf::Texture texture;
    if (!texture.loadFromFile(resolveProjectPath("src/images/placeholderSprite 32x32..png"))){
        throw std::runtime_error("Failed to create default texture.");
    }

    textures.emplace(DefaultTextureID, std::move(texture));
    texturePaths[DefaultTextureID] = "";
}
/**
 * @brief loads a sound buffer from a file and associates it with the given ID in the resource manager.
 * @param id The ID to associate with the loaded sound buffer.
 * @param filePath The path to the sound buffer file to load.
 * @return true if the sound buffer was successfully loaded and added to the resource manager, false otherwise.
 */
bool ResourceManager::loadSoundBuffer(const std::string &id, const std::string &filePath)
{
    if (hasSoundBuffer(id)) {
        soundBufferPaths[id] = filePath;
        return true;
    }
    sf::SoundBuffer soundBuffer;
    if (!soundBuffer.loadFromFile(resolveProjectPath(filePath))) {
        return false;
    }
    soundBuffers.emplace(id, std::move(soundBuffer));
    soundBufferPaths[id] = filePath;
    return true;
}
/**
 * @brief Retrieves a sound buffer associated with the given ID from the resource manager.
 * @param id The ID of the sound buffer to retrieve.
 * @return A reference to the sf::SoundBuffer object associated with the given ID.
 * @throws std::runtime_error if the sound buffer with the specified ID is not found.
 */
sf::SoundBuffer &ResourceManager::getSoundBuffer(const std::string &id)
{
    auto it = soundBuffers.find(id);
    if (it == soundBuffers.end()) {
        throw std::runtime_error("SoundBuffer not found: " + id);
    }
    return it->second;
}
/**
 * @brief Checks if a sound buffer with the given ID exists in the resource manager.
 * @param id The ID of the sound buffer to check.
 * @return true if the sound buffer exists, false otherwise.
 */
bool ResourceManager::hasSoundBuffer(const std::string &id) const
{
    return soundBuffers.find(id) != soundBuffers.end();
}
/**
 * @brief Loads a sound buffer manifest from a JSON file and adds the sound buffers to the resource manager.
 * @param filePath The path to the sound buffer manifest JSON file.
 * @return true if all sound buffers were successfully loaded, false otherwise.
 * @throws std::runtime_error if the manifest file cannot be opened or parsed.
 */
bool ResourceManager::loadSoundBufferManifest(const std::string &filePath)
{
    std::ifstream file(resolveProjectPath(filePath));
    if (!file.is_open()) {
        std::cerr << "Failed to open sound buffer manifest: " << filePath << std::endl;
        return false;
    }
    
    nlohmann::json manifestData;
    try {
        file >> manifestData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse sound buffer manifest: " << e.what() << std::endl;
        return false;
    }
    if (!manifestData.contains("soundBuffers") || !manifestData["soundBuffers"].is_array()) {
        std::cerr << "Sound buffer manifest has no soundBuffers array." << std::endl;
        return false;
    }
    bool loadedAllSoundBuffers = true;
    for (const auto& soundBufferData : manifestData["soundBuffers"]) {
        std::string id = soundBufferData.value("id", "");
        std::string path = soundBufferData.value("path", "");

        if (id.empty() || path.empty()) {
            std::cerr << "Invalid sound buffer manifest entry." << std::endl;
            loadedAllSoundBuffers = false;
            continue;
        }

        if (!loadSoundBuffer(id, path)) {
            std::cerr << "Failed to load sound buffer: " << id << " from " << path << std::endl;
            loadedAllSoundBuffers = false;
        }
    }
    return loadedAllSoundBuffers;
}

/**
 * @brief Loads a music file into the resource manager.
 * @param id The ID to associate with the music file.
 * @param filePath The path to the music file.
 * @return true if the music file was successfully loaded, false otherwise.
 */
bool ResourceManager::loadMusic(const std::string &id, const std::string &filePath)
{
    if (hasMusic(id)) {
        musicPaths[id] = filePath;
        return true;
    }
    std::ifstream file(resolveProjectPath(filePath));
    if (!file.is_open()) {
        std::cerr << "Failed to open music file: " << filePath << std::endl;
        return false;
    }
    musicPaths[id] = filePath;
    return true;
}
/**
 * @brief Checks if a music file with the given ID exists in the resource manager.
 * @param id The ID of the music file to check.
 * @return true if the music file exists, false otherwise.
 */
bool ResourceManager::hasMusic(const std::string &id) const
{
    return musicPaths.find(id) != musicPaths.end();
}
/**
 * @brief Loads a music manifest from a JSON file and adds the music files to the resource manager.
 * @param filePath The path to the music manifest JSON file.
 * @return true if all music files were successfully loaded, false otherwise.
 * @throws std::runtime_error if the manifest file cannot be opened or parsed.
 */
bool ResourceManager::loadMusicManifest(const std::string &filePath)
{
    std::ifstream file(resolveProjectPath(filePath));
    if (!file.is_open()) {
        std::cerr << "Failed to open music manifest: " << filePath << std::endl;
        return false;
    }
    
    nlohmann::json manifestData;
    try {
        file >> manifestData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse music manifest: " << e.what() << std::endl;
        return false;
    }
    if (!manifestData.contains("music") || !manifestData["music"].is_array()) {
        std::cerr << "Music manifest has no music array." << std::endl;
        return false;
    }
    bool loadedAllMusics = true;
    for (const auto& musicData : manifestData["music"]) {
        std::string id = musicData.value("id", "");
        std::string path = musicData.value("path", "");

        if (id.empty() || path.empty()) {
            std::cerr << "Invalid music manifest entry." << std::endl;
            loadedAllMusics = false;
            continue;
        }

        if (!loadMusic(id, path)) {
            std::cerr << "Failed to load music: " << id << " from " << path << std::endl;
            loadedAllMusics = false;
        }
    }
    return loadedAllMusics;
}
/**
 * @brief Loads a font from a file and associates it with the given ID in the resource manager.
 * @param id The ID to associate with the loaded font.
 * @param filePath The path to the font file to load.
 * @return true if the font was successfully loaded and added to the resource manager, false otherwise.
 * @throws std::runtime_error if the font cannot be loaded.
 */
bool ResourceManager::loadFont(const std::string &id, const std::string &filePath)
{
    if (hasFont(id)) {
        fontPaths[id] = filePath;
        return true;
    }
    sf::Font font;
    if (!font.openFromFile(resolveProjectPath(filePath))) {
        return false;
    }
    fonts.emplace(id, std::move(font));
    fontPaths[id] = filePath;
    return true;

}
/**
 * @brief Loads a font manifest from a JSON file and adds the fonts to the resource manager.
 * @param filePath The path to the font manifest JSON file.
 * @return true if all fonts were successfully loaded, false otherwise.
 * @throws std::runtime_error if the manifest file cannot be opened or parsed.
 */
bool ResourceManager::loadFontManifest(const std::string &filePath)
{
    std::ifstream file(resolveProjectPath(filePath));
    if (!file.is_open()) {
        std::cerr << "Failed to open font manifest: " << filePath << std::endl;
        return false;
    }

    nlohmann::json manifestData;
    try {
        file >> manifestData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse font manifest: " << e.what() << std::endl;
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
/**
 * @brief Retrieves a font associated with the given ID from the resource manager.
 * @param id The ID of the font to retrieve.
 * @return A reference to the sf::Font object associated with the given ID.
 * @throws std::runtime_error if the font with the specified ID is not found.
 */
sf::Font& ResourceManager::getFont(const std::string &id)
{
    auto it = fonts.find(id);
    if (it == fonts.end())
    {
        throw std::runtime_error("Font not found: " + id);
    }
    return it->second;
}
/**
 * @brief Checks if there is a loaded font associated with the given ID.
 * @param id The ID of the font to check the status of.
 * @return Returns true if the font exists, false if it doesnt.
 */
bool ResourceManager::hasFont(const std::string &id) const
{
    auto it = fonts.find(id);
    return (it != fonts.end());
}

/**
 * @brief Loads a default font into the application.
 */
void ResourceManager::createDefaultFont()
{
    if (hasFont(DefaultFontID)) {
        return;
    }

    sf::Font font;
    if (!font.openFromFile(resolveProjectPath("src/fonts/Times New Roman.ttf"))) {
        throw std::runtime_error("Failed to create default font.");
    }
    fonts.emplace(DefaultFontID, std::move(font));
    fontPaths[DefaultFontID] = "";
}

/**
 * @brief Loads a texture from a given file path and stores it under the given id.
 * @param id The ID the texture will be associated with.
 * @param filePath The location that the texture exists at in the file system.
 * @return Returns true if the texture is successfully loaded, false if otherwise.
 */
bool ResourceManager::loadTexture(const std::string &id, const std::string &filePath)
{
    if (hasTexture(id)) {
        texturePaths[id] = filePath;
        return true;
    }
    sf::Texture texture;
    if (!texture.loadFromFile(resolveProjectPath(filePath))) return false;
    textures.emplace(id, std::move(texture));
    texturePaths[id] = filePath;
    return true;
}
/**
 * @brief Loads a font manifest from a JSON file and loads the contents in the resource manager.
 * @param filePath Location of the manifest within the file system.
 * @return Returns true if the manifest is sucessfully loaded, false if otherwise.
 */
bool ResourceManager::loadTextureManifest(const std::string& filePath)
{
    std::ifstream file(resolveProjectPath(filePath));
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