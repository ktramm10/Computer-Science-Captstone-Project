#pragma once

#include "Component.h"
#include "Entity.h"
#include <memory>

class Scene;
class SceneLoader;
class ResourceManager;
class AudioSystem;

/**
 * @enum class InteractionType
 * @brief Enum representing different types of interactions that can occur between entities in the game.
 */
enum class InteractionType {
    PickUp,
    Key,
    Door
};

/**
 * @class Interactable
 * @brief Interactable component class that inherits from Component.
 * @details This class represents the interactable properties of an entity in a game. It contains various properties related to interactions,
 *  including the type of interaction and methods to handle interactions between entities.
 */
class Interactable : public Component {
    public:
    Interactable() {
        interactionType = InteractionType::PickUp;
    }
    InteractionType interactionType;
    Interactable(InteractionType interactionType) : interactionType(interactionType) {}
    bool interact(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> interactableEntity, Scene& scene,
            SceneLoader& sceneLoader, ResourceManager& resourceManager, AudioSystem& audioSystem);
    
};


