#include "CollisionSystem.h"
#include <cmath>
#include <iostream>

/**
 * @brief Anonymous namespace for internal helper functions.
 */
namespace {

Interactable interactable;
/**
 * @brief Retrieves the bounding rectangle of an entity based on its Transform and Collision components.
 * @param scene The scene containing the entity.
 * @param entity The entity for which to retrieve the bounding rectangle.
 * @return A FloatRect representing the bounding rectangle of the entity. If the entity does not have a Transform or Collision component, an empty rectangle is returned.
 */
sf::FloatRect getBounds(const Scene& scene, std::shared_ptr<Entity> entity) {
    if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision)) {
        auto transform = entity->getComponent<Transform>();
        auto collision = entity->getComponent<Collision>();

        sf::Vector2f position = scene.getWorldPosition(entity) + collision->offset;
        return sf::FloatRect(position, collision->size);
    }
    return sf::FloatRect(); // Return an empty rectangle if no Transform or Collision component is found
}
/**
 * @brief Checks if two rectangles have horizontal overlap.
 * @param a The first rectangle.
 * @param b The second rectangle.
 * @return True if the rectangles overlap horizontally, false otherwise.
 */
bool hasHorizontalOverlap(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x;
}
/**
 * @brief Determines if a rider entity is supported by a moving platform entity.
 * @param scene The scene containing the entities.
 * @param rider The entity representing the rider.
 * @param platform The entity representing the moving platform.
 * @return True if the rider is supported by the platform, false otherwise.
 */
bool isSupportedByPlatform(const Scene& scene, std::shared_ptr<Entity> rider, std::shared_ptr<Entity> platform) {
    if (!rider || !platform) {
        return false;
    }

    auto platformCollision = platform->getComponent<Collision>();
    if (!platformCollision || !platformCollision->isMovingPlatform) {
        return false;
    }

    sf::FloatRect riderBounds = getBounds(scene, rider);
    sf::FloatRect platformBounds = getBounds(scene, platform);
    float riderBottom = riderBounds.position.y + riderBounds.size.y;
    float platformTop = platformBounds.position.y;
    const float verticalTolerance = 4.0f;

    return hasHorizontalOverlap(riderBounds, platformBounds) &&
        std::abs(riderBottom - platformTop) <= verticalTolerance;
}
/**
 * @brief Clears unsupported moving platform parents for entities in the scene.
 * @param scene The scene containing the entities.
 */
void clearUnsupportedMovingPlatformParents(Scene& scene) {
    for (const auto& entity : scene.getEntities()) {
        if (!entity || !entity->isAlive() || entity->isBlueprint()) {
            continue;
        }

        auto parent = entity->getParentEntity().lock();
        if (!parent || isSupportedByPlatform(scene, entity, parent)) {
            continue;
        }

        auto parentCollision = parent->getComponent<Collision>();
        if (!parentCollision || !parentCollision->isMovingPlatform) {
            continue;
        }

        auto physics = entity->getComponent<Physics>();
        scene.clearParent(entity->getID());
        if (physics) {
            physics->useGravity = true;
            physics->isGrounded = false;
        }
    }
}
/**
 * @brief Handles hazard interactions between two entities in the scene.
 * @param entityA The first entity involved in the hazard interaction.
 * @param entityB The second entity involved in the hazard interaction.
 * @param scene The scene containing the entities.
 * @param scriptSystem The script system to handle events related to hazards.
 * @param dt The delta time for the current frame.
 */
void handleHazards(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, Scene& scene, ScriptSystem& scriptSystem, float dt)
{

    auto collisionA = entityA->getComponent<Collision>();
    auto collisionB = entityB->getComponent<Collision>();
    if (collisionA->faction == Faction::Terrain || collisionB->faction == Faction::Terrain) {
        return;
    }

    if (collisionA->faction != Faction::Neutral && collisionA->faction != collisionB->faction) {
        Hazard hazard;
        if (entityA->getComponent<ContactDamage>()) {
            hazard.applyHazard(entityB, entityA, scene, scriptSystem, dt);
        }
        if (entityB->getComponent<ContactDamage>()) {
            hazard.applyHazard(entityA, entityB, scene, scriptSystem, dt);
        }
    }
}
/**
 * @brief Handles interactions with interactable components between two entities in the scene.
 * @param entityA The first entity involved in the interaction.
 * @param entityB The second entity involved in the interaction.
 * @param scene The scene containing the entities.
 * @param sceneLoader The scene loader to handle scene transitions.
 * @param resourceManager The resource manager to handle resource loading.
 * @param scriptSystem The script system to handle events related to interactions.
 * @param dt The delta time for the current frame.
 */
void handleInteractables(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, Scene &scene,
    SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem, AudioSystem& audioSystem, float dt)
{
    bool hasKey = entityA->getComponent<Key>() || entityB->getComponent<Key>();
    bool hasPickUp = entityA->getComponent<PickUp>() || entityB->getComponent<PickUp>();
    if (hasKey || hasPickUp) {
        if (entityA->getComponent<Key>()) {
            interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager, audioSystem);
        }
        if (entityB->getComponent<Key>()) {
            interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager, audioSystem);
        }
        if (entityA->getComponent<PickUp>()) {
            interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager, audioSystem);
        }
        if (entityB->getComponent<PickUp>()) {
            interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager, audioSystem);
        }
        return;
    }

    auto doorA = entityA->getComponent<Door>();
    auto doorB = entityB->getComponent<Door>();
    if (doorA || doorB) {
        if (doorA) {
            interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager, audioSystem);
        }
        if (doorB) {
            interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager, audioSystem);
        }
    }

    auto switchComponentA = entityA->getComponent<Switch>();
    auto switchComponentB = entityB->getComponent<Switch>();
    if (switchComponentA || switchComponentB) {
        if (switchComponentA) {
            bool switched = interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager, audioSystem);
            if (switched) {
                if (switchComponentA->activated) scriptSystem.onSwitchActivate(scene, dt, entityA);
                else scriptSystem.onSwitchDeactivate(scene, dt, entityA);
            }
        }
        if (switchComponentB) {
            bool switched = interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager, audioSystem);
            if (switched) {
                if (switchComponentB->activated) scriptSystem.onSwitchActivate(scene, dt, entityB);
                else scriptSystem.onSwitchDeactivate(scene, dt, entityB); 
            }
        }
    }

    auto powerUpA = entityA->getComponent<PowerUp>();
    auto powerUpB = entityB->getComponent<PowerUp>();
    if (powerUpA || powerUpB) {
        if (powerUpA) {
            if (interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager, audioSystem)) {
                scriptSystem.OnPowerUpCollected(scene, dt, entityA);
            }
        }
        if (powerUpB) {
            if (interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager, audioSystem)) {
                scriptSystem.OnPowerUpCollected(scene, dt, entityB);
            }
        }
    }
}
/**
 * @brief Stops the velocity of a Physics component along the axes where a collision correction has occurred.
 * @param physics The Physics component whose velocity is to be modified.
 * @param correction The collision correction vector indicating the axes of collision.
 */
void stopVelocityOnCollisionAxis(std::shared_ptr<Physics> physics,const sf::Vector2f& correction)
{
    if (!physics) {
        return;
    }

    if (correction.x != 0.0f) {
        physics->velocity.x = 0.0f;
    }

    if (correction.y != 0.0f) {
        physics->velocity.y = 0.0f;
    }
}
/**
 * @brief Updates the grounded state of a Physics component based on the collision correction vector.
 * @param physics The Physics component whose grounded state is to be updated.
 * @param correction The collision correction vector indicating the direction of collision.
 */
void updateGroundedState(std::shared_ptr<Physics> physics, const sf::Vector2f& correction) 
{
    if (!physics) {
        return;
    }

    if (correction.y < 0.0f) {
        physics->isGrounded = true;
    }
}
/**
 * @brief Calculates the knockback correction vector based on the collision correction and knockback distance.
 * @param correction The collision correction vector indicating the direction of collision.
 * @param distance The knockback distance to be applied.
 * @return A vector representing the knockback correction to be applied to the entity.
 */
sf::Vector2f getKnockbackCorrection(const sf::Vector2f& correction, float distance) {
    if (distance <= 0.0f) {
        return {0.0f, 0.0f};
    }
    if (correction.x < 0.0f) {
        return {-distance, 0.0f};
    }
    if (correction.x > 0.0f) {
        return {distance, 0.0f};
    }
    if (correction.y < 0.0f) {
        return {0.0f, -distance};
    }
    if (correction.y > 0.0f) {
        return {0.0f, distance};
    }
    return {0.0f, 0.0f};
}
/**
 * @brief Calculates the fixed knockback vector based on the specified direction and distance.
 * @param direction The direction vector for the knockback.
 * @param distance The distance to apply the knockback.
 * @return A vector representing the fixed knockback to be applied to the entity. If the distance is zero or the direction is a zero vector, a zero vector is returned.
 */
sf::Vector2f getFixedKnockback(const sf::Vector2f& direction, float distance) {
    if (distance <= 0.0f || direction.lengthSquared() == 0.0f) {
        return {0.0f, 0.0f};
    }

    return direction.normalized() * distance;
}
/**
 * @brief Applies damage knockback to an entity based on its Transform, Collision, and Physics components.
 * @param transform The Transform component of the entity to apply knockback to.
 * @param collision The Collision component of the entity to apply knockback to.
 * @param physics The Physics component of the entity to apply knockback to.
 * @param knockback The knockback vector to be applied to the entity.
 * @param scene The scene containing the entity.
 */
void applyDamageKnockback(std::shared_ptr<Transform> transform, std::shared_ptr<Collision> collision, std::shared_ptr<Physics> physics, const sf::Vector2f& knockback, Scene& scene)
{
    if (!transform || !collision || collision->isStatic || knockback == sf::Vector2f{0.0f, 0.0f}) {
        return;
    }

    scene.moveEntity(transform->getParent(), knockback);
    stopVelocityOnCollisionAxis(physics, knockback);
}
/**
 * @brief Resolves the collision between two entities in the scene, applying necessary corrections and handling interactions.
 * @param entityA The first entity involved in the collision.
 * @param entityB The second entity involved in the collision.
 * @param intersection The intersection rectangle representing the overlapping area of the two entities.
 * @param scene The scene containing the entities.
 * @param sceneLoader The scene loader to handle scene transitions.
 * @param resourceManager The resource manager to handle resource loading.
 * @param scriptSystem The script system to handle events related to collisions.
 * @param dt The delta time for the current frame.
 */
void resolveCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, sf::FloatRect intersection,
     Scene& scene, SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem, AudioSystem& audioSystem, float dt) {
    // gather data for collision resolution
    handleInteractables(entityA, entityB, scene, sceneLoader, resourceManager, scriptSystem, audioSystem, dt);
    handleHazards(entityA, entityB, scene, scriptSystem, dt);
    
    auto transformA = entityA->getComponent<Transform>();
    auto transformB = entityB->getComponent<Transform>();
    auto collisionA = entityA->getComponent<Collision>();
    auto collisionB = entityB->getComponent<Collision>();
    auto physicsA = entityA->getComponent<Physics>();
    auto physicsB = entityB->getComponent<Physics>();

    // if resolving physical collisions with team is turned off then if they are on the same team, skip resolution of physical collision
    
    if (!collisionA->resolvePhysicalCollisionsWithTeam || !collisionB->resolvePhysicalCollisionsWithTeam) {
        if (collisionA-> faction != Faction::Neutral && collisionA->faction == collisionB->faction) {
            return;
        }
    }

    if (!transformA || !transformB || !collisionA || !collisionB) {
        return;
    }

    if (collisionA->isStatic && collisionB->isStatic) {
        return;
    }

    // determine overlap
    sf::FloatRect boundingBoxA = getBounds(scene, entityA);
    sf::FloatRect boundingBoxB = getBounds(scene, entityB);

    float overlapX = intersection.size.x;
    float overlapY = intersection.size.y;
    
    float centerAX = boundingBoxA.position.x + boundingBoxA.size.x / 2;
    float centerAY = boundingBoxA.position.y + boundingBoxA.size.y / 2;

    float centerBX = boundingBoxB.position.x + boundingBoxB.size.x / 2;
    float centerBY = boundingBoxB.position.y + boundingBoxB.size.y / 2;

    sf::Vector2f correction{0.0f, 0.0f};
    
    // collisions along the axis with a smaller overlap
    if (overlapX < overlapY) {
        if (centerAX < centerBX)
            correction.x = -overlapX;
        else
            correction.x = overlapX;
    }
    else {
        if (centerAY < centerBY)
            correction.y = -overlapY;
        else
            correction.y = overlapY;
    }

    auto contactDamageA = entityA->getComponent<ContactDamage>();
    auto contactDamageB = entityB->getComponent<ContactDamage>();
    
    sf::Vector2f knockbackA = getKnockbackCorrection(correction, collisionA->knockbackDistance);
    sf::Vector2f knockbackB = getKnockbackCorrection(-correction, collisionB->knockbackDistance);

    if (contactDamageA && contactDamageA->knockbackMethod == KnockbackMethod::Fixed) {
        knockbackB = getFixedKnockback(contactDamageA->knockbackDirection, collisionB->knockbackDistance);
    }
    if (contactDamageB && contactDamageB->knockbackMethod == KnockbackMethod::Fixed) {
        knockbackA = getFixedKnockback(contactDamageB->knockbackDirection, collisionA->knockbackDistance);
    }

    if (!collisionA->isPhysicalCollider || !collisionB->isPhysicalCollider) {
        applyDamageKnockback(transformA, collisionA, physicsA, knockbackA, scene);
        applyDamageKnockback(transformB, collisionB, physicsB, knockbackB, scene);
        collisionA->knockbackDistance = 0.0f;
        collisionB->knockbackDistance = 0.0f;
        return;
    }




    


    bool aStatic = collisionA->isStatic;
    bool bStatic = collisionB->isStatic;

    if (!aStatic && bStatic) {
        // A Moves B remains still
        sf::Vector2f totalCorrection = correction + knockbackA;
        scene.moveEntity(entityA, totalCorrection);
        stopVelocityOnCollisionAxis(physicsA, totalCorrection);
        updateGroundedState(physicsA, correction);
    }
    else if (aStatic && !bStatic)
    {
        sf::Vector2f totalCorrection = -correction + knockbackB;
        scene.moveEntity(entityB, totalCorrection);
        stopVelocityOnCollisionAxis(physicsB, totalCorrection);
        updateGroundedState(physicsB, -correction);
    }
    else {
        sf::Vector2f halfCorrection = correction / 2.0f;
        sf::Vector2f totalCorrectionA = halfCorrection + knockbackA;
        sf::Vector2f totalCorrectionB = -halfCorrection + knockbackB;

        scene.moveEntity(entityA, totalCorrectionA);
        scene.moveEntity(entityB, totalCorrectionB);

        stopVelocityOnCollisionAxis(physicsA, totalCorrectionA);
        stopVelocityOnCollisionAxis(physicsB, totalCorrectionB);

        updateGroundedState(physicsA, halfCorrection);
        updateGroundedState(physicsB, -halfCorrection);
    }
    collisionA->knockbackDistance = 0.0f;
    collisionB->knockbackDistance = 0.0f;

    if (collisionA->isMovingPlatform && !collisionB->isStatic && correction.y > 0) {
        if (physicsB && transformB && physicsB->velocity.y >= 0.0f && !entityB->getParentEntity().lock()) {
            
            std::cout << "Setting parent: " << entityB->getName() << " -> " << entityA->getName() << std::endl;
            physicsB->velocity.y = 0.0f;
            physicsB->useGravity = false;
            physicsB->isGrounded = true;
            scene.setParent(entityB->getID(), entityA->getID());
        }
    }
    else if (collisionB->isMovingPlatform && !collisionA->isStatic && correction.y < 0) {
        if (physicsA && transformA && physicsA->velocity.y >= 0.0f && !entityA->getParentEntity().lock()) {
            std::cout << "Setting parent: " << entityA->getName() << " -> " << entityB->getName() << std::endl;
            physicsA->velocity.y = 0.0f;
            physicsA->useGravity = false;
            physicsA->isGrounded = true;
            scene.setParent(entityA->getID(), entityB->getID());
        }
    }



}   
}

/**
 * @brief Updates the collision system for the given scene, handling collisions between entities and applying necessary corrections.
 * @param scene The scene containing the entities to be processed for collisions.
 * @param dt The delta time for the current frame.
 * @param sceneLoader The scene loader to handle scene transitions.
 * @param resourceManager The resource manager to handle resource loading.
 * @param scriptSystem The script system to handle events related to collisions.
 */
void CollisionSystem::update(Scene& scene, float dt, SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem, AudioSystem& audioSystem) {
    const auto& entities = scene.getEntities();

    for (size_t i = 0; i < entities.size(); ++i) {
        auto a = entities[i];
        if (!a || !a->isAlive() || a->isBlueprint()) {
            continue;
        }
        if ((a->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision))
        != (ComponentTypes::Transform | ComponentTypes::Collision)) {
            continue;
        }
        for (size_t j = i + 1; j < entities.size(); ++j) {
            auto b = entities[j];
            if (!a->isAlive()) {
                break;
            }
            if (!b || !b->isAlive() || b->isBlueprint()) {
                continue;
            }
            if ((b->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision))
        != (ComponentTypes::Transform | ComponentTypes::Collision)) {
            continue;
        }
            
            sf::FloatRect boundsA = getBounds(scene, a);
            sf::FloatRect boundsB = getBounds(scene, b);
            

            auto intersection = boundsA.findIntersection(boundsB);
            if (intersection) {
                sf::FloatRect intersectionRect = *intersection;
                scriptSystem.onCollision(scene, dt, b, a);
                resolveCollision(a, b, intersectionRect, scene, sceneLoader, resourceManager, scriptSystem, audioSystem, dt);
            }
        }
    }

    clearUnsupportedMovingPlatformParents(scene);
}

