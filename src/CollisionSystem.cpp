#include "CollisionSystem.h"
#include <iostream>

sf::FloatRect CollisionSystem::getBounds(std::shared_ptr<Entity> entity) {
    if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision)) {
        auto transform = entity->getComponent<Transform>();
        auto collision = entity->getComponent<Collision>();

        sf::Vector2f position = transform->position + collision->offset;
        return sf::FloatRect(position, collision->size);
    }
    return sf::FloatRect(); // Return an empty rectangle if no Transform or Collision component is found
}

void CollisionSystem::update(Scene& scene) {
    const auto& entities = scene.getEntities();

    for (size_t i = 0; i < entities.size(); ++i) {
        auto a = entities[i];
        if ((a->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision))
        != (ComponentTypes::Transform | ComponentTypes::Collision)) {
            continue;
        }
        for (size_t j = i + 1; j < entities.size(); ++j) {
            auto b = entities[j];
            if ((b->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Collision))
        != (ComponentTypes::Transform | ComponentTypes::Collision)) {
            continue;
        }
            
            sf::FloatRect boundsA = getBounds(a);
            sf::FloatRect boundsB = getBounds(b);
            

            auto intersection = boundsA.findIntersection(boundsB);
            if (intersection) {
                if (a->getComponent<Collision>()->isTrigger || b->getComponent<Collision>()->isTrigger) {

                // determine which entity is the interacter and which is the interactee
                if (a->getComponent<Interactable>() && b->getComponent<Interactable>()) {
                    // both entities are interactable, skip interaction
                    continue;
                }
                else if (a->getComponent<Interactable>()) {
                    std::cout << "Entity " << a->getName() << " is interacting with Entity " << b->getName() << std::endl;
                    a->getComponent<Interactable>()->interact(b, a, scene);
                }
                else if (b->getComponent<Interactable>()) {
                    std::cout << "Entity " << a->getName() << " is interacting with Entity " << b->getName() << std::endl;
                    b->getComponent<Interactable>()->interact(a, b, scene);
                }
            } else {
                sf::FloatRect intersectionRect = *intersection;
                resolveCollision(a, b, intersectionRect, scene);
            }
            }
        }
    }
}

void CollisionSystem::resolveCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB, sf::FloatRect intersection, Scene& scene) {
    if (entityA->getComponent<Collision>()->isTrigger || entityB->getComponent<Collision>()->isTrigger) {
        if (entityA->getComponent<Interactable>() && entityB->getComponent<Interactable>()) {
            // both entities are interactable, skip interaction
            return;
        }
        else if (entityA->getComponent<Interactable>()) {
            std::cout << "Entity " << entityA->getName() << " is interacting with Entity " << entityB->getName() << std::endl;
            entityA->getComponent<Interactable>()->interact(entityB, entityA, scene);
        }
        else if (entityB->getComponent<Interactable>()) {
            std::cout << "Entity " << entityB->getName() << " is interacting with Entity " << entityA->getName() << std::endl;
            entityB->getComponent<Interactable>()->interact(entityA, entityB, scene);
        }
        return; // Skip collision resolution for triggers
    }
    
    // gather data for collision resolution
    auto transformA = entityA->getComponent<Transform>();
    auto transformB = entityB->getComponent<Transform>();
    auto collisionA = entityA->getComponent<Collision>();
    auto collisionB = entityB->getComponent<Collision>();
    auto physicsA = entityA->getComponent<Physics>();
    auto physicsB = entityB->getComponent<Physics>();


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
    
    float centerAX = boundingBoxA.position.x + boundingBoxB.size.x / 2;
    float centerAY = boundingBoxA.position.y + boundingBoxB.size.y / 2;

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

    // static object will not be displaced on collision
    bool aStatic = collisionA->isStatic;
    bool bStatic = collisionB->isStatic;

    if (!aStatic && bStatic) {
        // A Moves B remains still
        transformA->position += correction;
        stopVelocityOnCollisionAxis(physicsA, correction);
        updateGroundedState(physicsA, correction);
    }
    else if (aStatic && !bStatic)
    {
        // B Moves A remains still
        transformB->position += correction;
        stopVelocityOnCollisionAxis(physicsB, correction);
        updateGroundedState(physicsB, correction);
    }
    else {
        // if both objects move dynamically we split the correction 50 50
        sf::Vector2f halfCorrection = correction / 2.0f;

        transformA->position += halfCorrection;
        transformB->position -= halfCorrection;

        stopVelocityOnCollisionAxis(physicsA, halfCorrection);
        stopVelocityOnCollisionAxis(physicsB, -halfCorrection);

        updateGroundedState(physicsA, halfCorrection);
        updateGroundedState(physicsB, -halfCorrection);
    }




}   

// stop the current velocity along the axis which the correction is made
void CollisionSystem::stopVelocityOnCollisionAxis(std::shared_ptr<Physics> physics,const sf::Vector2f& correction)
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
// if the object is corrected upward it has landed on something so set the grounded state
void CollisionSystem::updateGroundedState(std::shared_ptr<Physics> physics, const sf::Vector2f& correction) 
{
    if (!physics) {
        return;
    }

    if (correction.y < 0.0f){
        physics->isGrounded = true;
    }
}

