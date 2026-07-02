#include "Interactable.h"
#include "Entity.h"
#include "Scene.h"

void Interactable::interact(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> interactableEntity, Scene &scene)
{
    auto collectable = interactableEntity->getComponent<Collectable>();
    if (collectable && otherEntity == scene.getPlayerEntity()) {
        switch (collectable->collectableType) {
            case CollectableType::Health: {
                auto health = otherEntity->getComponent<Health>();
                if (health) {
                    health->heal(collectable->amount);
                }
                break;
            }
            case CollectableType::Ammo:
                // Handle ammo collection
                break;
            case CollectableType::Coin:
                scene.getGameState().coins += static_cast<int>(collectable->amount);
                break;
        }
        interactableEntity->destroy(); // Destroy the collectable entity after interaction
    }
}