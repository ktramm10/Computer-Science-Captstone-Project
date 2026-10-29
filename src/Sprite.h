#pragma once

#include "Component.h"
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @class Sprite
 * @brief Component that stores an SFML sprite and the texture ID used to create it.
 */
class Sprite : public Component
{
    public:
    sf::Sprite *sprite;
    std::string texID;

    /**
     * @brief Constructs a sprite component from a texture and texture ID.
     * @param texture The texture used by the sprite.
     * @param texID The resource ID associated with the texture.
     */
    Sprite(const sf::Texture& texture, const std::string texID) {
        this->texID = texID;
        sprite = new sf::Sprite(texture);
    }

    /**
     * @brief Constructs a sprite component from a texture with a placeholder texture ID.
     * @param texture The texture used by the sprite.
     */
    Sprite(const sf::Texture& texture) {
        texID = "0000";
        sprite = new sf::Sprite(texture);
    }

    /**
     * @brief Constructs a sprite component that stores a texture ID without creating an SFML sprite.
     * @param texID The resource ID associated with the texture.
     */
    Sprite(const std::string texID) : texID(texID){}

    /**
     * @brief Sets the texture rectangle used by the wrapped SFML sprite.
     * @param rectangle The texture rectangle to apply.
     */
    void setTextureRect(const sf::IntRect& rectangle) {
        sprite->setTextureRect(rectangle);
    }
};