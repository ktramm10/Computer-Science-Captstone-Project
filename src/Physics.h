#pragma once
#include "Component.h"
#include "Transform.h"
class Physics : public Component
{
    public:
    sf::Vector2f velocity{0.0f, 0.0f};
    sf::Vector2f acceleration{0.0f, 0.0f};
    float gravity = 400.0f;

    bool isGrounded = false;
    bool useGravity = true;


    Physics() = default;
    ~Physics() override = default;

    Physics(sf::Vector2f velocity, sf::Vector2f acceleration, float gravity) : velocity(velocity), acceleration(acceleration), gravity(gravity) {}
};