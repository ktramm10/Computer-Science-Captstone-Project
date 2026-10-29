#pragma once

#include "Component.h"
#include "Factions.h"
#include <SFML/Graphics.hpp>

/**
 * @class Collision
 * @brief Collision component class that inherits from Component.
 * @details This class represents the collision properties of an entity in a game. It contains various properties related to collision detection and response,
 *  including size, offset, knockback distance, static state, physical collider state, faction, and whether to resolve physical collisions with team members.
 */
class Collision : public Component
{
    
    public:
    sf::Vector2f size{32.f, 32.f};
    sf::Vector2f offset{0.f, 0.f};
    float knockbackDistance = 0.f;

    
    bool isStatic = false;
    bool isPhysicalCollider = true;
    bool resolvePhysicalCollisionsWithTeam = true;
    bool isMovingPlatform = false;
    Faction faction = Faction::Neutral;

    Collision() = default;
    Collision(const sf::Vector2f& size, const sf::Vector2f& offset = {0.f, 0.f}, bool isStatic = false, float knockback = 0.f, bool isPhysicalCollider = true, Faction faction = Faction::Neutral, bool resolvePhysicalCollisionsWithTeam = true)
       : size(size), offset(offset), knockbackDistance(knockback), isStatic(isStatic), isPhysicalCollider(isPhysicalCollider), resolvePhysicalCollisionsWithTeam(resolvePhysicalCollisionsWithTeam), faction(faction) {}

};