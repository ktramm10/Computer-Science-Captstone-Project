#include "SceneSerializer.h"
#include "GameConfig.h"
#include "ProjectPaths.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

/**
 * @brief Converts an SFML vector into a JSON array.
 * @param vector The vector to serialize.
 * @return A JSON array containing the vector's x and y values.
 */
json SceneSerializer::writeVec2(const sf::Vector2f &vector) const
{
    return json::array({vector.x, vector.y});
}

/**
 * @brief Serializes an entity and all supported components to JSON.
 * @param entity The entity to serialize.
 * @param scene The scene containing the entity and parent/player context.
 * @return A JSON object representing the entity.
 */
json SceneSerializer::saveEntity(const std::shared_ptr<Entity> &entity, const Scene& scene) const
{
    json entityData;
    entityData["name"] = entity->getName();
    entityData["isPlayer"] = entity == scene.getPlayerEntity();
    entityData["isBlueprint"] = entity->isBlueprint();
    entityData["id"] = entity->getID();
    entityData["parentID"] = entity->getParentEntity().lock() ? entity->getParentEntity().lock()->getID() : -1;
    entityData["components"] = json::object();
    json& components = entityData["components"];

    saveTransformComponent(components, entity, scene);
    saveSpriteComponent(components, entity);
    saveCollisionComponent(components, entity);
    savePhysicsComponent(components, entity);
    saveInputComponent(components, entity);
    saveHealthComponent(components, entity);
    savePickUpComponent(components, entity);
    saveContactDamageComponent(components, entity);
    saveAIComponent(components, entity);
    saveShooterComponent(components, entity);
    saveLifeTimeComponent(components, entity);
    saveSpawnerComponent(components, entity);
    saveDoorComponent(components, entity);
    saveKeyComponent(components, entity);
    saveScriptComponent(components, entity);
    saveTextComponent(components, entity);
    saveSwitchComponent(components, entity);
    savePowerUpComponent(components, entity);
    savePopTextComponent(components, entity);
    saveLootComponent(components, entity);
    saveSoundComponent(components, entity);

    return entityData;
}

/**
 * @brief Saves an entity's Transform component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 * @param scene The scene used to calculate world position.
 */
void SceneSerializer::saveTransformComponent(json &components, const std::shared_ptr<Entity> &entity, const Scene& scene) const
{
    if ((entity->getComponentMask() & ComponentTypes::Transform) != ComponentTypes::Transform) {
        return;
    }

    auto transform = entity->getComponent<Transform>();

    if (!transform) return;

    components["transform"] = {
        {"position", writeVec2(scene.getWorldPosition(entity))}, 
        {"localPosition", writeVec2(transform->getLocalPosition())},
        {"scale", writeVec2(transform->getScale())}, 
        {"rotation", transform->getRotation()}
    };
}

/**
 * @brief Saves an entity's Collision component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveCollisionComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Collision) != ComponentTypes::Collision) {
        return;
    }
    auto collision = entity->getComponent<Collision>();

    if (!collision) return;

    components["collision"] = {
        {"size", writeVec2(collision->size)},
        {"offset", writeVec2(collision->offset)},
        {"isStatic", collision->isStatic},
        {"isPhysicalCollider", collision->isPhysicalCollider},
        {"resolvePhysicalCollisionsWithTeam", collision->resolvePhysicalCollisionsWithTeam},
        {"isMovingPlatform", collision->isMovingPlatform},
        {"faction", static_cast<int>(collision->faction)},
        {"knockbackDistance", collision->knockbackDistance}
    };
}

/**
 * @brief Saves an entity's Sprite component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveSpriteComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Sprite) != ComponentTypes::Sprite) {
        return;
    }
    auto sprite = entity->getComponent<Sprite>();
    if (!sprite) return;

    components["sprite"] = {
        {"texture", sprite->texID }
    };
}

/**
 * @brief Saves an entity's Physics component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::savePhysicsComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Physics) != ComponentTypes::Physics) {
        return;
    }
    auto physics = entity->getComponent<Physics>();
    if (!physics) return;

    components["physics"] = {
        {"velocity", writeVec2(physics->velocity)},
        {"acceleration", writeVec2(physics->acceleration)},
        {"gravity", physics->gravity},
        {"isGrounded", physics->isGrounded},
        {"useGravity", physics->useGravity},
        {"useAcceleration", physics->useAcceleration}
    };
}

/**
 * @brief Saves an entity's Input component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveInputComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Input) != ComponentTypes::Input) {
        return;
    }
    auto input = entity->getComponent<Input>();
    if (!input) return;

    components["input"] = {
        {"movementType", static_cast<int>(input->movementType)},
        {"moveSpeed", input->moveSpeed},
        {"acceleration", input->acceleration},
        {"deceleration", input->deceleration},
        {"jumpForce", input->jumpForce},
        {"canJump", input->canJump},
        {"canMove", input->canMove}
    };
}

/**
 * @brief Saves an entity's Health component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveHealthComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Health) != ComponentTypes::Health) {
        return;
    }
    auto health = entity->getComponent<Health>();
    if (!health) return;

    components["health"] = {
        {"currentHealth", health->currentHealth},
        {"maxHealth", health->maxHealth}
    };
}

/**
 * @brief Saves an entity's PickUp component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::savePickUpComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::PickUp) != ComponentTypes::PickUp) {
        return;
    }
    auto pickUp = entity->getComponent<PickUp>();
    if (!pickUp) return;

    components["pickUp"] = {
        {"pickUpType", static_cast<int>(pickUp->pickUpType)},
        {"amount", pickUp->amount}
    };
}

/**
 * @brief Saves an entity's ContactDamage component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveContactDamageComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::ContactDamage) != ComponentTypes::ContactDamage) {
        return;
    }
    auto contactDamage = entity->getComponent<ContactDamage>();
    if (!contactDamage) return;

    components["contactDamage"] = {
        {"damage", contactDamage->damage},
        {"knockback", contactDamage->knockback},
        {"knockbackMethod", static_cast<int>(contactDamage->knockbackMethod)},
        {"knockbackDirection", writeVec2(contactDamage->knockbackDirection)},
        {"destroyEntityOnContact", contactDamage->destroyEntityOnContact}
    };
}

/**
 * @brief Saves an entity's AI component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveAIComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::AI) != ComponentTypes::AI) {
        return;
    }
    auto ai = entity->getComponent<AI>();
    if (!ai) return;

    components["ai"] = {
        {"movementType", static_cast<int>(ai->movementType)},
        {"moveSpeed", ai->moveSpeed},
        {"acceleration", ai->acceleration},
        {"deceleration", ai->deceleration},
        {"aggroRange", ai->aggroRange},
        {"canFollow", ai->canFollow},
        {"defaultMovementState", static_cast<int>(ai->defaultMovementState)},
        {"currentMovementState", static_cast<int>(ai->currentMovementState)},
        {"verticalStrafe", ai->verticalStrafe},
        {"strafeDistance", ai->strafeDistance},
        {"strafeStartX", ai->strafeStartX},
        {"strafeStartY", ai->strafeStartY},
        {"strafeDirection", ai->strafeDirection},
        {"hasStrafeStart", ai->hasStrafeStart},
        {"goalPosition", writeVec2(ai->goalPosition)},
        {"shootTimeLeft", ai->shootTimeLeft},
        {"shootFrequency", ai->shootFrequency},
        {"errorMargin", ai->errorMargin},
        {"decelerating", ai->decelerating}
    };
}

/**
 * @brief Saves an entity's Shooter component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveShooterComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Shooter) != ComponentTypes::Shooter) {
        return;
    }
    auto shooter = entity->getComponent<Shooter>();
    if (!shooter) return;

    components["shooter"] = {
        {"textureID", shooter->textureID},
        {"gunMode", static_cast<int>(shooter->gunMode)},
        {"projectileSpeed", shooter->projectileSpeed},
        {"projectileDamage", shooter->projectileDamage},
        {"shotGunPelletCount", shooter->shotGunPelletCount},
        {"projectileDirection", writeVec2(shooter->projectileDirection)},
        {"projectileScale", writeVec2(shooter->projectileScale)},
        {"offset", writeVec2(shooter->offset)}
    };
}

/**
 * @brief Saves an entity's LifeTime component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveLifeTimeComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::LifeTime) != ComponentTypes::LifeTime) {
        return;
    }
    auto lifeTime = entity->getComponent<LifeTime>();
    if (!lifeTime) return;

    components["lifeTime"] = {
        {"lifeTimeDuration", lifeTime->lifeTimeDuration},
        {"lifeTimeTimer", lifeTime->lifeTimeTimer}
    };
}

/**
 * @brief Saves an entity's Spawner component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveSpawnerComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Spawner) != ComponentTypes::Spawner) {
        return;
    }
    auto spawner = entity->getComponent<Spawner>();
    if (!spawner) return;

    components["spawner"] = {
        {"baseName", spawner->baseName},
        {"spawnMethod", static_cast<int>(spawner->spawnMethod)},
        {"spawnFrequency", spawner->spawnFrequency},
        {"spawnTimeLeft", spawner->spawnTimeLeft},
        {"spawnPosition", writeVec2(spawner->spawnPosition)}
    };
}

/**
 * @brief Saves an entity's Door component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveDoorComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Door) != ComponentTypes::Door) {
        return;
    }
    auto door = entity->getComponent<Door>();
    if (!door) return;

    components["door"] = {
        {"sceneFilePath", door->sceneFilePath},
        {"keyId", door->keyId},
        {"isLocked", door->isLocked},
        {"unlockedTexID", door->unlockedTexID}
    };
}

/**
 * @brief Saves an entity's Key component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveKeyComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Key) != ComponentTypes::Key) {
        return;
    }
    auto key = entity->getComponent<Key>();
    if (!key) return;

    components["key"] = {
        {"keyId", key->keyId}
    };
}

/**
 * @brief Saves an entity's Script component and rules into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveScriptComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Script) != ComponentTypes::Script) {
        return;
    }
    auto script = entity->getComponent<Script>();
    if (!script) return;

    json rules = json::array();
    for (const auto& rule : script->rules) {
        json actions = json::array();
        for (const auto& action : rule.actions) {
            actions.push_back({
                {"type", static_cast<int>(action.type)},
                {"x", action.x},
                {"y", action.y},
                {"value", action.value},
                {"target", action.target}
            });
        }

        rules.push_back({
            {"event", static_cast<int>(rule.event)},
            {"actions", actions}
        });
    }

    components["script"] = {
        {"started", script->started},
        {"filePath", script->filePath},
        {"fileName", script->fileName},
        {"rules", rules}
    };
}

/**
 * @brief Saves an entity's Text component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveTextComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Text) == 0) {
        return;
    }
    auto text = entity->getComponent<Text>();
    if (!text) return;
    components["text"] = {
        {"font", text->fontID},
        {"style", static_cast<int>(text->style)},
        {"linkGameValue", text->linkGameValue},
        {"positionRelativeToViewport", text->positionRelativeToViewport},
        {"linkedGameValue", static_cast<int>(text->linkedGameValue)},
        {"characterSize", text->characterSize},
        {"textString", text->textString},
        {"fillColorR", text->fillColor.r},
        {"fillColorG", text->fillColor.g},
        {"fillColorB", text->fillColor.b},
        {"fillColorA", text->fillColor.a}
    };
}

/**
 * @brief Saves an entity's Switch component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveSwitchComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Switch) != ComponentTypes::Switch) {
        return;
    }
    auto switchComponent = entity->getComponent<Switch>();
    if (!switchComponent) return;

    components["switch"] = {
        {"activated", switchComponent->activated},
        {"activatedTextureID", switchComponent->activatedTextureID},
        {"deactivatedTextureID", switchComponent->deactivatedTextureID}
    };
}

/**
 * @brief Saves an entity's PowerUp component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::savePowerUpComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::PowerUp) != ComponentTypes::PowerUp) {
        return;
    }
    auto powerUp = entity->getComponent<PowerUp>();
    if (!powerUp) return;

    components["powerUp"] = {
        {"velocityBoost", powerUp->velocityBoost},
        {"moveSpeedBoost", powerUp->moveSpeedBoost},
        {"powerUpDuration", powerUp->powerUpDuration},
        {"powerUpTimeLeft", powerUp->powerUpTimeLeft},
        {"powerUpType", static_cast<int>(powerUp->powerUpType)},
        {"selectionMethod", static_cast<int>(powerUp->selectionMethod)}
    };
}

/**
 * @brief Saves an entity's PopText component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::savePopTextComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::PopText) != ComponentTypes::PopText) {
        return;
    }
    auto popText = entity->getComponent<PopText>();
    if (!popText) return;

    components["popText"] = {
        {"font", popText->fontID},
        {"offset", writeVec2(popText->offset)},
        {"style", static_cast<int>(popText->style)},
        {"characterSize", popText->size},
        {"textString", popText->textString},
        {"fillColorR", popText->color.r},
        {"fillColorG", popText->color.g},
        {"fillColorB", popText->color.b},
        {"fillColorA", popText->color.a},
        {"lifeTimeDuration", popText->lifeTimeDuration}
    };
}

/**
 * @brief Saves an entity's Loot component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveLootComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Loot) != ComponentTypes::Loot) {
        return;
    }
    auto loot = entity->getComponent<Loot>();
    if (!loot) return;

    components["loot"] = {
        {"gold", loot->gold},
        {"score", loot->score},
        {"experience", loot->experience}
    };
}

/**
 * @brief Saves an entity's Sound component into the component JSON object.
 * @param components The JSON object receiving component data.
 * @param entity The entity being serialized.
 */
void SceneSerializer::saveSoundComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Sound) != ComponentTypes::Sound) {
        return;
    }
    auto sound = entity->getComponent<Sound>();
    if (!sound) return;

    components["sound"] = {
        {"soundBuffer", sound->soundBufferID}
    };
}

/**
 * @brief Saves the entire scene to a JSON file.
 * @param filePath The path of the scene file to write.
 * @param scene The scene to serialize.
 * @param resourceManager The resource manager associated with the scene.
 * @return True if the scene was saved successfully, false otherwise.
 */
bool SceneSerializer::saveToFile(const std::string &filePath, const Scene &scene, const ResourceManager &resourceManager) const
{
    json sceneData;
    sceneData["gravity"] = GRAVITY;
    sceneData["viewport_camera_speed"] = VIEWPORT_CAMERA_SPEED;
    sceneData["viewport_camera_offset"] = writeVec2(VIEWPORT_CAMERA_OFFSET);
    sceneData["viewport_center_position"] = writeVec2(VIEWPORT_CENTER_POSITION);
    sceneData["viewport_follow_player_character"] = VIEWPORT_FOLLOW_PLAYER_CHARACTER;
    sceneData["music_file_path"] = MUSIC_FILE_PATH;
    sceneData["window_size"] = writeVec2(WINDOW_SIZE);
    sceneData["backgroundColor"] = json::array({
        scene.backgroundColor.r,
        scene.backgroundColor.g,
        scene.backgroundColor.b,
        scene.backgroundColor.a
    });

    sceneData["entities"] = json::array();
    for (const auto& entity : scene.getEntities()) {
        sceneData["entities"].push_back(saveEntity(entity, scene));
    }
    std::ofstream file(resolveProjectPath(filePath));

    if (!file.is_open()) {
        std::cerr << "Failed to open scene file for saving: " << filePath << std::endl;
        return false;
    }
    file << sceneData.dump(4);
    return true;
}
