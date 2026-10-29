#pragma once
#include "Component.h"
#include "GameConfig.h"
#include <SFML/System/Vector2.hpp>

/**
 * @class Physics
 * @brief Physics component class that inherits from Component.
 * @details This class represents the physics properties of an entity in a game. It contains various properties related to the entity's movement and interaction with forces.
 */
class Physics : public Component
{
    public:
    sf::Vector2f velocity{0.0f, 0.0f};
    sf::Vector2f acceleration{0.0f, 0.0f};
    float gravity = GRAVITY;

    bool isGrounded = false;
    bool useGravity = true;
    bool useAcceleration = true;


    Physics() = default;
    ~Physics() override = default;

    Physics(sf::Vector2f velocity, sf::Vector2f acceleration, float gravity) : velocity(velocity), acceleration(acceleration), gravity(gravity) {}
};