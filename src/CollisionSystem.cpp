#include "CollisionSystem.h"
#include <cmath>
#include <iostream>


namespace {

Interactable interactable;
sf::FloatRect getBounds(const Scene& scene, std::shared_ptr<Entity> entity) {
    if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision)) {
        auto transform = entity->getComponent<Transform>();
        auto collision = entity->getComponent<Collision>();

        sf::Vector2f position = scene.getWorldPosition(entity) + collision->offset;
        return sf::FloatRect(position, collision->size);
    }
    return sf::FloatRect(); // Return an empty rectangle if no Transform or Collision component is found
}

bool hasHorizontalOverlap(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x;
}

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

void applyDamageKnockback(std::shared_ptr<Transform> transform, std::shared_ptr<Collision> collision, std::shared_ptr<Physics> physics, const sf::Vector2f& knockback, Scene& scene)
{
    if (!transform || !collision || collision->isStatic || knockback == sf::Vector2f{0.0f, 0.0f}) {
        return;
    }

    scene.moveEntity(transform->getParent(), knockback);
    stopVelocityOnCollisionAxis(physics, knockback);
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




    


    // static object will not be displaced on collision
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
        // B Moves A remains still
        sf::Vector2f totalCorrection = -correction + knockbackB;
        scene.moveEntity(entityB, totalCorrection);
        stopVelocityOnCollisionAxis(physicsB, totalCorrection);
        updateGroundedState(physicsB, -correction);
    }
    else {
        // if both objects move dynamically we split the correction 50 50
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
            
            sf::FloatRect boundsA = getBounds(scene, a);
            sf::FloatRect boundsB = getBounds(scene, b);
            

            auto intersection = boundsA.findIntersection(boundsB);
            if (intersection) {
                sf::FloatRect intersectionRect = *intersection;
                scriptSystem.onCollision(scene, dt, b, a);
                resolveCollision(a, b, intersectionRect, scene, sceneLoader, resourceManager, scriptSystem, dt);
            }
        }
    }

    clearUnsupportedMovingPlatformParents(scene);
}

