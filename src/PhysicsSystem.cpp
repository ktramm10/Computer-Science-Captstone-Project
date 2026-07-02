#include "PhysicsSystem.h"
#include <iostream>

void PhysicsSystem::update(Scene& scene, float dt) {
    // Update logic for the scene, e.g., updating entities, physics, etc.
    for (auto& entity : scene.getEntities()) {
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
            transform->position += physics->velocity * dt;
        }
    }
}