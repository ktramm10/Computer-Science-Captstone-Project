#include "PhysicsSystem.h"
#include <iostream>

/**
 * @brief Updates the physics system for a given scene, applying physics calculations to entities with Transform and Physics components.
 * @param scene The scene containing the entities to be processed for physics updates.
 * @param dt The delta time since the last update, used for timing updates.
 */
void PhysicsSystem::update(Scene& scene, float dt) {
    // Update logic for the scene, e.g., updating entities, physics, etc.
    for (auto& entity : scene.getEntities()) {
        if (entity->isBlueprint()) {
            continue;
        }
        const auto componentMask = entity->getComponentMask();
        if ((componentMask & ComponentTypes::Transform) != 0 && (componentMask & ComponentTypes::Physics) != 0) {
            auto physics = entity->getComponent<Physics>();
            auto transform = entity->getComponent<Transform>();

            // Update velocity based on acceleration
            physics->velocity += physics->acceleration * dt;
            // Apply gravity (only if enabled)
            if (physics->useGravity) {
                physics->velocity.y += physics->gravity * dt;
            }

            // Update position based on velocity
            scene.moveEntity(entity, physics->velocity * dt);
        }
    }
}