#include "Interactable.h"
#include "Entity.h"
#include "SceneLoader.h"
#include "Scene.h"
#include "EditorUI.h"
#include "Files.h"
#include "AudioSystem.h"
#include <iostream>

/**
 * @brief Handles the interaction between an entity and an interactable entity in the scene, performing actions based on the type of interaction.
 * @param otherEntity The entity that is interacting with the interactable entity.
 * @param interactableEntity The entity that has the Interactable component and is being interacted with.
 * @param scene The scene containing the entities and their components.
 * @param sceneLoader The scene loader to handle scene transitions.
 * @param resourceManager The resource manager to handle resource loading.
 * @return A boolean indicating whether the interaction resulted in a state change (e.g., switch
 */
bool Interactable::interact(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> interactableEntity, Scene &scene,
    SceneLoader& sceneLoader, ResourceManager& resourceManager, AudioSystem& audioSystem) 
{
    bool switched = false;
    bool powerUpActivated = false;



    auto pickUp = interactableEntity->getComponent<PickUp>();
    if (pickUp && otherEntity == scene.getPlayerEntity()) {
        switch (pickUp->pickUpType) {
            case PickUpType::Health: {
                auto health = otherEntity->getComponent<Health>();
                if (health) {
                    health->heal(pickUp->amount);
                }
                break;
            }
            case PickUpType::Ammo:
                // Handle ammo collection
                break;
            case PickUpType::Coin:
                scene.getGameState().coins += static_cast<int>(pickUp->amount);
                if (auto sound = interactableEntity->getComponent<Sound>()) {
                    audioSystem.playSound(resourceManager.getSoundBuffer(sound->soundBufferID));
                }
                break;
        }
        interactableEntity->destroy();
    }

    auto key = interactableEntity->getComponent<Key>();
    if (key && otherEntity == scene.getPlayerEntity()) {
        scene.getGameState().keyIDs.push_back(key->keyId);
        scene.updateDoorLockStates();
        interactableEntity->destroy(); 
    }
    

    auto door = interactableEntity->getComponent<Door>();
    if (door && otherEntity == scene.getPlayerEntity()) {
        if (!door->isLocked) {
            // check if file is valid
            if (Files::isValidJsonFile(door->sceneFilePath)) {
                scene.requestSceneLoad(door->sceneFilePath);
            }
        }
    }

    auto switchComponent = interactableEntity->getComponent<Switch>();
    auto spriteComponent = interactableEntity->getComponent<Sprite>();
    if (spriteComponent && switchComponent && otherEntity == scene.getPlayerEntity()) {
        if (switchComponent->lockout > 0.0f) {
            switchComponent->lockout = switchComponent->lockoutTimer;
            return false;
        }
        if (switchComponent->activated) {
            switchComponent->activated = false;
            spriteComponent->texID = switchComponent->deactivatedTextureID;
            spriteComponent->sprite->setTexture(resourceManager.getTexture(switchComponent->deactivatedTextureID));
            switchComponent->lockout = switchComponent->lockoutTimer;
            switched = true;
            
        }
        else {
            switchComponent->activated = true;
            spriteComponent->texID = switchComponent->activatedTextureID;
            spriteComponent->sprite->setTexture(resourceManager.getTexture(switchComponent->activatedTextureID));
            switchComponent->lockout = switchComponent->lockoutTimer;
            switched = true;
            
        }
        return switched;
    }

    auto powerUp = interactableEntity->getComponent<PowerUp>();
    auto input = otherEntity->getComponent<Input>();
    auto shooter = otherEntity->getComponent<Shooter>();
    if (input && shooter && powerUp && otherEntity == scene.getPlayerEntity()) {
        powerUp->activated = true;
        powerUp->getParent()->setBlueprint(true);
        switch(powerUp->powerUpType) {
            case PowerUpType::MoveSpeed:
                input->moveSpeed += powerUp->moveSpeedBoost;
                input->acceleration += powerUp->moveSpeedBoost;
                input->deceleration += powerUp->moveSpeedBoost;
                break;
            case PowerUpType::VelocityBoost:
                shooter->projectileSpeed += powerUp->velocityBoost;
                break;
            case PowerUpType::Shotgun:
                shooter->gunMode = GunMode::Shotgun;
                break;
        }
        powerUp->powerUpTimeLeft = powerUp->powerUpDuration;
        powerUpActivated = true;
        return powerUpActivated;
    }   
    return false;
}