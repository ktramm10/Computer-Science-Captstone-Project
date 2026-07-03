#pragma once

#include "Component.h"
#include "Entity.h"
#include <memory>

class Scene;

enum class InteractionType {
    Collectable,
    Dialogue,
};

enum class CollectableType {
    Coin,
    Health,
    Ammo
};

class Interactable : public Component {
    public:
    Interactable() = default;
    InteractionType interactionType;
    Interactable(InteractionType interactionType) : interactionType(interactionType) {}
    void interact(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> interactableEntity, Scene& scene);
    
};


