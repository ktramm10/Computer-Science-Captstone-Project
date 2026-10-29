#pragma once

#include "Scene.h"

/**
 * @class PhysicsSystem
 * @brief Physics system class that handles physics calculations for entities in a scene.
 * @details This class provides functionality to update the physics system for a given scene, applying physics calculations to entities with Transform and Physics components.
 */
class PhysicsSystem {
    public:
    void update(Scene& scene, float dt);
};