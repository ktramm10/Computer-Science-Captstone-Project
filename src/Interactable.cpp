#include "Interactable.h"
#include "Entity.h"
#include "Scene.h"
#include <iostream>
void Interactable::interact(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> interactableEntity, Scene &scene)
{
    auto collectable = interactableEntity->getComponent<Collectable>();
    if (collectable && otherEntity == scene.getPlayerEntity()) {
        switch (collectable->collectableType) {
            case CollectableType::Health: {
                auto health = otherEntity->getComponent<Health>();
                if (health) {
                    std::cout << "Player collected health: " << collectable->amount << std::endl;
                    health->heal(collectable->amount);
                }
                break;
            }
            case CollectableType::Ammo:
                // Handle ammo collection
                std::cout << "Player collected ammo: " << collectable->amount << std::endl;
                break;
            case CollectableType::Coin:
                std::cout << "Player collected coins: " << collectable->amount << std::endl;
                scene.getGameState().coins += static_cast<int>(collectable->amount);
                break;
        }
        interactableEntity->destroy(); // Destroy the collectable entity after interaction
    }
}