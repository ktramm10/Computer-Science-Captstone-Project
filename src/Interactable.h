#pragma once

#include "Component.h"
#include "Entity.h"
#include <memory>

class Scene;
class SceneLoader;
class ResourceManager;

enum class InteractionType {
    PickUp,
    Key,
    Door
};

class Interactable : public Component {
    public:
    Interactable() {
        interactionType = InteractionType::PickUp;
    }
    InteractionType interactionType;
    Interactable(InteractionType interactionType) : interactionType(interactionType) {}
    bool interact(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> interactableEntity, Scene& scene,
         SceneLoader& sceneLoader, ResourceManager& resourceManager);
    
};


