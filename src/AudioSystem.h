#pragma once

#include <SFML/Audio.hpp>
#include <memory>
#include <vector>

/**
 * @class AudioSystem
 * @brief Owns short-lived sounds so one-shot effects can keep playing after their source entity is destroyed.
 */
class AudioSystem {
public:
    void playSound(sf::SoundBuffer& soundBuffer);
    void update();

private:
    std::vector<std::unique_ptr<sf::Sound>> activeSounds;
};