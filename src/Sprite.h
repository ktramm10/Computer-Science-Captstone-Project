#pragma once

#include "Component.h"
#include <SFML/Graphics.hpp>
#include <string>

class Sprite : public Component
{
    public:
    sf::Sprite *sprite;
    std::string texID;

    Sprite(const sf::Texture& texture, const std::string texID) {
        this->texID = texID;
        sprite = new sf::Sprite(texture);
    }
    Sprite(const sf::Texture& texture) {
        texID = "0000";
        sprite = new sf::Sprite(texture);
    }

    Sprite(const std::string texID) : texID(texID){}

    void setTextureRect(const sf::IntRect& rectangle) {
        sprite->setTextureRect(rectangle);
    }
};