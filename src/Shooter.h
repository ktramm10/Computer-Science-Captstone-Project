#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>
#include <string>

/**
 * @brief Enum representing available projectile firing modes.
 */
enum class GunMode {
    default,
    Shotgun
};

/**
 * @class Shooter
 * @brief Component that stores projectile firing settings for an entity.
 */
class Shooter : public Component {
    public:
    /**
     * @brief Constructs a shooter component with default projectile settings.
     */
    Shooter() = default;
    GunMode gunMode = GunMode::default;
    float shotGunPelletCount = 5.f;
    float projectileSpeed = 400.f;
    float projectileDamage = 5.f;
    sf::Vector2f projectileDirection{1.0f, 0.0f};
    std::string textureID = "default";
    // scaling from 32x32 base
    sf::Vector2f projectileScale{1.0f, 1.0f};
    sf::Vector2f collisionScaling{32.f * projectileScale.x, 32.f * projectileScale.y};
    sf::Vector2f offset{0.f, 0.f};
};