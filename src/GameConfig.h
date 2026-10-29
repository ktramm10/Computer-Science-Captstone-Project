#pragma once

#include "ProjectPaths.h"

#include <SFML/System/Vector2.hpp>
#include <string>

/**
 * @brief Global configuration settings for the game.
 * @details This header file defines various global configuration settings for the game, including gravity, camera
 */

inline float GRAVITY = 300.f; 
inline float VIEWPORT_CAMERA_SPEED = 300.f;
inline sf::Vector2f VIEWPORT_CAMERA_OFFSET = {0.f, 0.f};
inline sf::Vector2f VIEWPORT_CENTER_POSITION = {960.f, 540.f};
inline bool VIEWPORT_FOLLOW_PLAYER_CHARACTER = true;
inline std::string MUSIC_FILE_PATH = resolveProjectPath("src/assets/music/background_music_test.ogg").string();
inline int MUSIC_VOLUME = 50;
inline sf::Vector2f WINDOW_SIZE = {1920.f, 1080.f};