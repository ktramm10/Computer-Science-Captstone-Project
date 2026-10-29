#include "AudioSystem.h"

#include <algorithm>

void AudioSystem::playSound(sf::SoundBuffer& soundBuffer)
{
    auto sound = std::make_unique<sf::Sound>(soundBuffer);
    sound->play();
    activeSounds.push_back(std::move(sound));
}

void AudioSystem::update()
{
    activeSounds.erase(std::remove_if(activeSounds.begin(), activeSounds.end(),
        [](const std::unique_ptr<sf::Sound>& sound) {
            return sound->getStatus() == sf::SoundSource::Status::Stopped;
        }), activeSounds.end());
}