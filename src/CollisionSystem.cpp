#include "CollisionSystem.h"
#include <iostream>


namespace {

Interactable interactable;
sf::FloatRect getBounds(std::shared_ptr<Entity> entity) {
    if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision)) {
        auto transform = entity->getComponent<Transform>();
        auto collision = entity->getComponent<Collision>();

        sf::Vector2f position = transform->getWorldPosition() + collision->offset;
        return sf::FloatRect(position, collision->size);
    }
    return sf::FloatRect(); // Return an empty rectangle if no Transform or Collision component is found
}

void handleHazards(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, Scene& scene, ScriptSystem& scriptSystem, float dt)
{

    auto collisionA = entityA->getComponent<Collision>();
    auto collisionB = entityB->getComponent<Collision>();

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
void handleInteractables(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, Scene &scene,
     SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem, float dt)
{
    bool hasKey = entityA->getComponent<Key>() || entityB->getComponent<Key>();
    bool hasPickUp = entityA->getComponent<PickUp>() || entityB->getComponent<PickUp>();
    if (hasKey || hasPickUp) {
        if (entityA->getComponent<Key>()) {
            interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager);
        }
        if (entityB->getComponent<Key>()) {
            interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager);
        }
        if (entityA->getComponent<PickUp>()) {
            interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager);
        }
        if (entityB->getComponent<PickUp>()) {
            interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager);
        }
        return;
    }

    auto doorA = entityA->getComponent<Door>();
    auto doorB = entityB->getComponent<Door>();
    if (doorA || doorB) {
        if (doorA) {
            interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager);
        }
        if (doorB) {
            interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager);
        }
    }

    auto switchComponentA = entityA->getComponent<Switch>();
    auto switchComponentB = entityB->getComponent<Switch>();
    if (switchComponentA || switchComponentB) {
        if (switchComponentA) {
            bool switched = interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager);
            if (switched) {
                if (switchComponentA->activated) scriptSystem.onSwitchActivate(scene, dt, entityA);
                else scriptSystem.onSwitchDeactivate(scene, dt, entityA);
            }
        }
        if (switchComponentB) {
            bool switched = interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager);
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
            if (interactable.interact(entityB, entityA, scene, sceneLoader, resourceManager)) {
                scriptSystem.OnPowerUpCollected(scene, dt, entityA);
            }
        }
        if (powerUpB) {
            if (interactable.interact(entityA, entityB, scene, sceneLoader, resourceManager)) {
                scriptSystem.OnPowerUpCollected(scene, dt, entityB);
            }
        }
    }
}

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

void updateGroundedState(std::shared_ptr<Physics> physics, const sf::Vector2f& correction) 
{
    if (!physics) {
        return;
    }

    if (correction.y < 0.0f) {
        physics->isGrounded = true;
    }
}

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

sf::Vector2f getFixedKnockback(const sf::Vector2f& direction, float distance) {
    if (distance <= 0.0f || direction.lengthSquared() == 0.0f) {
        return {0.0f, 0.0f};
    }

    return direction.normalized() * distance;
}



void resolveCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, sf::FloatRect intersection,
     Scene& scene, SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem, float dt) {
    // gather data for collision resolution
    handleInteractables(entityA, entityB, scene, sceneLoader, resourceManager, scriptSystem, dt);
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

    // breakout checks
    if (!transformA || !transformB || !collisionA || !collisionB) {
        return;
    }

    if (collisionA->isStatic && collisionB->isStatic) {
        return;
    }

    // determine overlap
    sf::FloatRect boundingBoxA = getBounds(entityA);
    sf::FloatRect boundingBoxB = getBounds(entityB);

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
        return;
    }
    // static object will not be displaced on collision
    bool aStatic = collisionA->isStatic;
    bool bStatic = collisionB->isStatic;

    if (!aStatic && bStatic) {
        // A Moves B remains still
        sf::Vector2f totalCorrection = correction + knockbackA;
        transformA->moveWorldPosition(totalCorrection);
        stopVelocityOnCollisionAxis(physicsA, totalCorrection);
        updateGroundedState(physicsA, correction);
    }
    else if (aStatic && !bStatic)
    {
        // B Moves A remains still
        sf::Vector2f totalCorrection = -correction + knockbackB;
        transformB->moveWorldPosition(totalCorrection);
        stopVelocityOnCollisionAxis(physicsB, totalCorrection);
        updateGroundedState(physicsB, -correction);
    }
    else {
        // if both objects move dynamically we split the correction 50 50
        sf::Vector2f halfCorrection = correction / 2.0f;
        sf::Vector2f totalCorrectionA = halfCorrection + knockbackA;
        sf::Vector2f totalCorrectionB = -halfCorrection + knockbackB;

        transformA->moveWorldPosition(totalCorrectionA);
        transformB->moveWorldPosition(totalCorrectionB);

        stopVelocityOnCollisionAxis(physicsA, totalCorrectionA);
        stopVelocityOnCollisionAxis(physicsB, totalCorrectionB);

        updateGroundedState(physicsA, halfCorrection);
        updateGroundedState(physicsB, -halfCorrection);
    }
    collisionA->knockbackDistance = 0.0f;
    collisionB->knockbackDistance = 0.0f;




}   
}


void CollisionSystem::update(Scene& scene, float dt, SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem) {
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
            
            sf::FloatRect boundsA = getBounds(a);
            sf::FloatRect boundsB = getBounds(b);
            

            auto intersection = boundsA.findIntersection(boundsB);
            if (intersection) {
                sf::FloatRect intersectionRect = *intersection;
                scriptSystem.onCollision(scene, dt, b, a);
                resolveCollision(a, b, intersectionRect, scene, sceneLoader, resourceManager, scriptSystem, dt);
            }
        }
    }
}

