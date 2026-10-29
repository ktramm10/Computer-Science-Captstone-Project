#pragma once
#include "Component.h"
#include "SFML/Audio.hpp"
#include <memory>
#include <string>

class Sound : public Component {
    public:
    std::shared_ptr<sf::Sound> sound;
    std::string soundBufferID;

    Sound(sf::SoundBuffer& soundBuffer, const std::string& soundBufferID) {
        this->soundBufferID = soundBufferID;
        sound = std::make_shared<sf::Sound>(soundBuffer);
    }

    void setSoundBuffer(sf::SoundBuffer& soundBuffer, const std::string& soundBufferID) {
        this->soundBufferID = soundBufferID;
        if (!sound) {
            sound = std::make_shared<sf::Sound>(soundBuffer);
            return;
        }
        sound->setBuffer(soundBuffer);
    }

    void play() {
        if (sound) {
            sound->play();
        }
    }

    void stop() {
        if (sound) {
            sound->stop();
        }
    }
};