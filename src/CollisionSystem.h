#pragma once

#include <SFML/Graphics.hpp>

#include "Scene.h"
#include "Entity.h"

class CollisionSystem {
    private:
    sf::FloatRect getBounds(std::shared_ptr<Entity> entity);
    void resolveCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, sf::FloatRect intersection, Scene& scene);
    void stopVelocityOnCollisionAxis(std::shared_ptr<Physics> physics,const sf::Vector2f& correction);
    void updateGroundedState(std::shared_ptr<Physics> physics, const sf::Vector2f& correction);
    public:
    void update(Scene& scene);
};