#pragma once

#include "Component.h"
#include "cmath"



class Transform : public Component
{
    public:
    sf::Vector2f position;
    float rotation = 0.0f;
    sf::Vector2f scale = sf::Vector2f(1.0f, 1.0f);

    Transform() = default;
    Transform(sf::Vector2f position, sf::Vector2f scale, float rotation )
        : position(position), scale(scale), rotation(rotation)  {}
};