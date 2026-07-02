#pragma once

#include "Component.h"
#include "Entity.h"
#include <string>
#include <memory>

class Interactable : public Component {
    public:
    Interactable() = default;
    Interactable(const std::string& interactionType) : interactionType(interactionType) {}
    virtual void interact(std::shared_ptr<Entity> interactee, std::shared_ptr<Entity> interacter, Scene& scene) {}
    std::string interactionType;
};


// must be a better way to do this, need a way to differentiate between collectable subtypes such as health, ammo, etc.
enum InteractionType {
    CollectableInteraction,
    DialogueInteraction,
};