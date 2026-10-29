#include "SceneLoader.h"

#include "GameConfig.h"
#include "ProjectPaths.h"
#include "ResourceManager.h"
#include "Scene.h"

#include <fstream>
#include <iostream>

/**
 * @brief Loads a scene from a JSON file into the provided scene instance.
 * @param filePath The path to the scene JSON file.
 * @param scene The scene to populate from the file.
 * @param resourceManager The resource manager used to resolve component assets.
 * @return True if the scene was loaded successfully, false otherwise.
 */
bool SceneLoader::loadFromFile(const std::string &filePath, Scene &scene, ResourceManager &resourceManager)
{
    const auto resolvedFilePath = resolveProjectPath(filePath);
    std::ifstream file(resolvedFilePath);
    if (!file.is_open()) {
    std::cerr << "Failed to open scene file: " << filePath << std::endl;
    return false;
    }

    json sceneData;
    try {
        file >> sceneData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse scene Json: " << e.what() << std::endl;
        return false;
    }

    scene.clear();
    GRAVITY = sceneData.value("gravity", GRAVITY);
    VIEWPORT_CAMERA_SPEED = sceneData.value("viewport_camera_speed", VIEWPORT_CAMERA_SPEED);
    VIEWPORT_CAMERA_OFFSET = readVec2(sceneData, "viewport_camera_offset", VIEWPORT_CAMERA_OFFSET);
    VIEWPORT_CENTER_POSITION = readVec2(sceneData, "viewport_center_position", VIEWPORT_CENTER_POSITION);
    MUSIC_FILE_PATH = resolveProjectPath(sceneData.value("music_file_path", MUSIC_FILE_PATH)).string();
    VIEWPORT_FOLLOW_PLAYER_CHARACTER = sceneData.value("viewport_follow_player_character", VIEWPORT_FOLLOW_PLAYER_CHARACTER);
    WINDOW_SIZE = readVec2(sceneData, "window_size", WINDOW_SIZE);
    if (sceneData.contains("backgroundColor") && sceneData["backgroundColor"].is_array() && sceneData["backgroundColor"].size() >= 4) {
        const auto& color = sceneData["backgroundColor"];
        scene.backgroundColor = sf::Color(
            color[0].get<std::uint8_t>(),
            color[1].get<std::uint8_t>(),
            color[2].get<std::uint8_t>(),
            color[3].get<std::uint8_t>()
        );
    }
    if (!sceneData.contains("entities") || !sceneData["entities"].is_array()) {
        std::cerr << "Scene file has no entities array." << std::endl;
        return false;
    }

    for (const auto& entityData : sceneData["entities"]) {
        loadEntity(entityData, scene, resourceManager);
    }
    for (const auto& entityData : sceneData["entities"]) {
        int childID = entityData.value("id", -1);
        int parentID = entityData.value("parentID", -1);
        if (childID != -1 && parentID != -1) {
            scene.setParent(static_cast<unsigned int>(childID), static_cast<unsigned int>(parentID));
        }
    }
    scene.setCurrentScenePath(resolvedFilePath);
    
    return true;
}

/**
 * @brief Reads a two-dimensional vector from a JSON array.
 * @param object The JSON object containing the vector field.
 * @param key The key of the vector field to read.
 * @param defaultValue The value to return if the field is missing or invalid.
 * @return The parsed vector value, or the default value if parsing is not possible.
 */
sf::Vector2f SceneLoader::readVec2(const json& object, const std::string& key, const sf::Vector2f& defaultValue) const {
    if (!object.contains(key) || !object[key].is_array()) {
        return defaultValue;
    }
    const auto& array = object[key];
    if (array.size() < 2) {
        return defaultValue;
    }

    return sf::Vector2f{array[0].get<float>(), array[1].get<float>()};
}

/**
 * @brief Creates an entity from JSON and loads each serialized component on it.
 * @param entityData The JSON data for the entity.
 * @param scene The scene receiving the entity.
 * @param resourceManager The resource manager used for asset-backed components.
 */
void SceneLoader::loadEntity(const json &entityData, Scene &scene, ResourceManager &resourceManager)
{
    std::string name = entityData.value("name", "entity");
    auto entity = entityData.contains("id")
        ? scene.createEntityWithID(entityData.value("id", 0u), name)
        : scene.createEntity(name);
    entity->setBlueprint(entityData.value("isBlueprint", false));
    bool isPlayer = entityData.value("isPlayer", false);
    if (isPlayer && !entity->isBlueprint()) {
        scene.setPlayerEntity(entity);
    }

    if (!entityData.contains("components") || !entityData["components"].is_object()) {
        return;
    }

    const auto& components = entityData["components"];
    if (components.contains("transform")) {
        loadTransform(components["transform"], entity);
    }
    if (components.contains("sprite")) {
        loadSprite(components["sprite"], entity, resourceManager);
    }
    if (components.contains("collision")) {
        loadCollision(components["collision"], entity);
    }
    if (components.contains("physics")) {
        loadPhysics(components["physics"], entity);
    }
    if (components.contains("input")) {
        loadInput(components["input"], entity);
    }
    if (components.contains("health")) {
        loadHealth(components["health"], entity);
    }
    if (components.contains("pickUp")) {
        loadPickUp(components["pickUp"], entity);
    }
    if (components.contains("contactDamage")) {
        loadContactDamage(components["contactDamage"], entity);
    }
    if (components.contains("ai")) {
        loadAI(components["ai"], entity);
    }
    if (components.contains("shooter")) {
        loadShooter(components["shooter"], entity);
    }
    if (components.contains("lifeTime")) {
        loadLifeTime(components["lifeTime"], entity);
    }
    if (components.contains("spawner")) {
        loadSpawner(components["spawner"], entity, scene);
    }
    if (components.contains("door")) {
        loadDoor(components["door"], entity);
    }
    if (components.contains("key")) {
        loadKey(components["key"], entity);
    }
    if (components.contains("script")) {
        loadScript(components["script"], entity);
    }
    if (components.contains("text")) {
        loadText(components["text"], entity, resourceManager);
    }
    if (components.contains("switch")) {
        loadSwitch(components["switch"], entity);
    }
    if (components.contains("powerUp")) {
        loadPowerUp(components["powerUp"], entity);
    }
    if (components.contains("popText")) {
        loadPopText(components["popText"], entity, resourceManager);
    }
    if (components.contains("loot")) {
        loadLoot(components["loot"], entity);
    }
    if (components.contains("sound")) {
        loadSound(components["sound"], entity, resourceManager);
    }
}

/**
 * @brief Loads a Transform component from JSON onto an entity.
 * @param componentData The JSON data for the transform component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadTransform(const json &componentData, std::shared_ptr<Entity> entity)
{
    sf::Vector2f position = readVec2(componentData, "position", {0.0f, 0.0f});
    sf::Vector2f localPosition = readVec2(componentData, "localPosition", {0.f, 0.f});
    sf::Vector2f scale = readVec2(componentData, "scale", {1.f, 1.f});
    float rotation = componentData.value("rotation", 0.0f);
    auto transform = std::make_shared<Transform>(position, scale, rotation);
    transform->setLocalPosition(localPosition);
    entity->addComponent(transform, ComponentTypes::Transform);
}

/**
 * @brief Loads a Sprite component from JSON onto an entity.
 * @param componentData The JSON data for the sprite component.
 * @param entity The entity receiving the component.
 * @param resourceManager The resource manager used to retrieve the sprite texture.
 */
void SceneLoader::loadSprite(const json &componentData, std::shared_ptr<Entity> entity, ResourceManager& resourceManager)
{
    std::string textureID = componentData.value("texture", "");
    if (textureID.empty()) {
        std::cerr << "Sprite component missing texture ID." << std::endl;
        return;
    }
    try {
        auto sprite = std::make_shared<Sprite>(resourceManager.getTexture(textureID), textureID);
        entity->addComponent(sprite, ComponentTypes::Sprite);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create sprite component: " << e.what() << std::endl;
    }

}

/**
 * @brief Loads a Collision component from JSON onto an entity.
 * @param componentData The JSON data for the collision component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadCollision(const json &componentData, std::shared_ptr<Entity> entity)
{
    sf::Vector2f size = readVec2(componentData, "size", {32.f, 32.f});
    sf::Vector2f offset = readVec2(componentData, "offset", {0.0f, 0.0f});
    float knockback = componentData.value("knockbackDistance", componentData.value("knockbackScalar", 0.0f));
    bool isStatic = componentData.value("isStatic", false);
    bool isPhysicalCollider = componentData.value("isPhysicalCollider", true);
    Faction faction = static_cast<Faction>(componentData.value("faction", static_cast<int>(Faction::Neutral)));
    bool resolvePhysicalCollisionsWithTeam = componentData.value("resolvePhysicalCollisionsWithTeam", true);
    bool isMovingPlatform = componentData.value("isMovingPlatform", false);

    auto collision = std::make_shared<Collision>(size, offset, isStatic, knockback, isPhysicalCollider, faction, resolvePhysicalCollisionsWithTeam);
    collision->isMovingPlatform = isMovingPlatform;
    entity->addComponent(collision, ComponentTypes::Collision);
}

/**
 * @brief Loads a Physics component from JSON onto an entity.
 * @param componentData The JSON data for the physics component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadPhysics(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto physics = std::make_shared<Physics>();
    physics->velocity = readVec2(componentData, "velocity", {0.0f, 0.0f});
    physics->acceleration = readVec2(componentData, "acceleration", {0.0f, 0.0f});
    physics->gravity = componentData.value("gravity", GRAVITY);
    physics->isGrounded = componentData.value("isGrounded", false);
    physics->useGravity = componentData.value("useGravity", true);
    physics->useAcceleration = componentData.value("useAcceleration", false);
    entity->addComponent(physics, ComponentTypes::Physics);
}

/**
 * @brief Loads an Input component from JSON onto an entity.
 * @param componentData The JSON data for the input component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadInput(const json &componentData, std::shared_ptr<Entity> entity)
{
    float moveSpeed = componentData.value("moveSpeed", 100.0f);
    float jumpForce = componentData.value("jumpForce", 300.0f);
    MovementType movementType = static_cast<MovementType>(componentData.value("movementType", static_cast<int>(MovementType::Platformer)));

    auto input = std::make_shared<Input>(moveSpeed, jumpForce, movementType);
    input->acceleration = componentData.value("acceleration", 50.f);
    input->deceleration = componentData.value("deceleration", 50.f);
    input->canJump = componentData.value("canJump", true);
    input->canMove = componentData.value("canMove", true);
    entity->addComponent(input, ComponentTypes::Input);
}

/**
 * @brief Loads a Health component from JSON onto an entity.
 * @param componentData The JSON data for the health component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadHealth(const json &componentData, std::shared_ptr<Entity> entity)
{
    float maxHealth = componentData.value("maxHealth", 100.0f);
    float currentHealth = componentData.value("currentHealth", maxHealth);

    auto health = std::make_shared<Health>(maxHealth, currentHealth);
    entity->addComponent(health, ComponentTypes::Health);
}

/**
 * @brief Loads a PickUp component from JSON onto an entity.
 * @param componentData The JSON data for the pickup component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadPickUp(const json &componentData, std::shared_ptr<Entity> entity)
{
    int pickUpTypeValue = componentData.value("pickUpType", componentData.value("itemType", static_cast<int>(PickUpType::Coin)));
    PickUpType pickUpType = static_cast<PickUpType>(pickUpTypeValue);
    float amount = componentData.value("amount", 1.0f);
    auto pickUp = std::make_shared<PickUp>(pickUpType, amount);
    entity->addComponent(pickUp, ComponentTypes::PickUp);
}

/**
 * @brief Loads a ContactDamage component from JSON onto an entity.
 * @param componentData The JSON data for the contact damage component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadContactDamage(const json &componentData, std::shared_ptr<Entity> entity)
{
    float damage = componentData.value("damage", 0.0f);
    float knockback = componentData.value("knockback", 0.0f);
    auto contactDamage = std::make_shared<ContactDamage>(damage, knockback);
    contactDamage->knockbackMethod = static_cast<KnockbackMethod>(componentData.value("knockbackMethod", static_cast<int>(contactDamage->knockbackMethod)));
    contactDamage->knockbackDirection = readVec2(componentData, "knockbackDirection", contactDamage->knockbackDirection);
    contactDamage->destroyEntityOnContact = componentData.value("destroyEntityOnContact", false);
    entity->addComponent(contactDamage, ComponentTypes::ContactDamage);
}

/**
 * @brief Loads an AI component from JSON onto an entity.
 * @param componentData The JSON data for the AI component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadAI(const json &componentData, std::shared_ptr<Entity> entity)
{
    float moveSpeed = componentData.value("moveSpeed", 300.0f);
    float aggroRange = componentData.value("aggroRange", 300.0f);
    float strafeDistance = componentData.value("strafeDistance", 150.0f);
    MovementType movementType = static_cast<MovementType>(componentData.value("movementType", static_cast<int>(MovementType::Platformer)));
    PlatformerMovementStates defaultMovementState = static_cast<PlatformerMovementStates>(componentData.value("defaultMovementState", static_cast<int>(PlatformerMovementStates::Strafe)));

    auto ai = std::make_shared<AI>();
    ai->movementType = movementType;
    ai->moveSpeed = moveSpeed;
    ai->acceleration = componentData.value("acceleration", ai->acceleration);
    ai->deceleration = componentData.value("deceleration", ai->deceleration);
    ai->aggroRange = aggroRange;
    ai->strafeDistance = strafeDistance;
    ai->strafeStartX = componentData.value("strafeStartX", ai->strafeStartX);
    ai->strafeStartY = componentData.value("strafeStartY", ai->strafeStartY);
    ai->verticalStrafe = componentData.value("verticalStrafe", ai->verticalStrafe);
    ai->hasStrafeStart = componentData.value("hasStrafeStart", ai->hasStrafeStart);
    ai->defaultMovementState = defaultMovementState;
    ai->canFollow = componentData.value("canFollow", false);
    ai->currentMovementState = static_cast<PlatformerMovementStates>(componentData.value("currentMovementState", static_cast<int>(defaultMovementState)));
    ai->strafeDirection = componentData.value("strafeDirection", 1);
    ai->goalPosition = readVec2(componentData, "goalPosition", ai->goalPosition);
    ai->shootTimeLeft = componentData.value("shootTimeLeft", ai->shootTimeLeft);
    ai->shootFrequency = componentData.value("shootFrequency", ai->shootFrequency);
    ai->errorMargin = componentData.value("errorMargin", ai->errorMargin);
    ai->decelerating = componentData.value("decelerating", ai->decelerating);
    entity->addComponent(ai, ComponentTypes::AI);
}

/**
 * @brief Loads a Shooter component from JSON onto an entity.
 * @param componentData The JSON data for the shooter component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadShooter(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto shooter = std::make_shared<Shooter>();
    shooter->gunMode = static_cast<GunMode>(componentData.value("gunMode", static_cast<int>(shooter->gunMode)));
    shooter->projectileSpeed = componentData.value("projectileSpeed", 400.0f);
    shooter->projectileDamage = componentData.value("projectileDamage", 5.0f);
    shooter->shotGunPelletCount = componentData.value("shotGunPelletCount", 5.f);
    shooter->projectileDirection = readVec2(componentData, "projectileDirection", {0.0f, 0.0f});
    shooter->textureID = componentData.value("textureID", ResourceManager::DefaultTextureID);
    shooter->projectileScale = readVec2(componentData, "projectileScale", {1.0f, 1.0f});
    shooter->collisionScaling = {32.f * shooter->projectileScale.x, 32.f * shooter->projectileScale.y};
    shooter->offset = readVec2(componentData, "offset", {0.0f, 0.0f});
    entity->addComponent(shooter, ComponentTypes::Shooter);
}

/**
 * @brief Loads a LifeTime component from JSON onto an entity.
 * @param componentData The JSON data for the lifetime component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadLifeTime(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto lifeTime = std::make_shared<LifeTime>(componentData.value("lifeTimeDuration", 0.0f));
    lifeTime->lifeTimeTimer = componentData.value("lifeTimeTimer", 0.0f);
    entity->addComponent(lifeTime, ComponentTypes::LifeTime);
}

/**
 * @brief Loads a Spawner component from JSON onto an entity.
 * @param componentData The JSON data for the spawner component.
 * @param entity The entity receiving the component.
 * @param scene The scene that owns the spawned entity context.
 */
void SceneLoader::loadSpawner(const json &componentData, std::shared_ptr<Entity> entity, Scene& scene)
{
    auto spawner = std::make_shared<Spawner>();
    spawner->baseName = componentData.value("baseName", "");
    spawner->spawnMethod = static_cast<SpawnMethod>(componentData.value("spawnMethod", static_cast<int>(SpawnMethod::Random)));
    spawner->spawnFrequency = componentData.value("spawnFrequency", 0.0f);
    spawner->spawnTimeLeft = componentData.value("spawnTimeLeft", 0.0f);
    spawner->spawnPosition = readVec2(componentData, "spawnPosition", {0.0f, 0.0f});

    entity->addComponent(spawner, ComponentTypes::Spawner);
}

/**
 * @brief Loads a Door component from JSON onto an entity.
 * @param componentData The JSON data for the door component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadDoor(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto door = std::make_shared<Door>();
    door->sceneFilePath = componentData.value("sceneFilePath", "");
    door->keyId = componentData.value("keyId", "default");
    door->isLocked = componentData.value("isLocked", false);
    door->unlockedTexID = componentData.value("unlockedTexID", "");
    entity->addComponent(door, ComponentTypes::Door);
}

/**
 * @brief Loads a Key component from JSON onto an entity.
 * @param componentData The JSON data for the key component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadKey(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto key = std::make_shared<Key>();
    key->keyId = componentData.value("keyId", "default");
    entity->addComponent(key, ComponentTypes::Key);
}

/**
 * @brief Loads a Script component and its rules from JSON onto an entity.
 * @param componentData The JSON data for the script component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadScript(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto script = std::make_shared<Script>();
    script->started = componentData.value("started", false);
    script->filePath = componentData.value("filePath", "");
    script->fileName = componentData.value("fileName", "");

    if (componentData.contains("rules") && componentData["rules"].is_array()) {
        for (const auto& ruleData : componentData["rules"]) {
            ScriptRule rule;
            rule.event = static_cast<ScriptEvent>(ruleData.value("event", static_cast<int>(ScriptEvent::OnUpdate)));

            if (ruleData.contains("actions") && ruleData["actions"].is_array()) {
                for (const auto& actionData : ruleData["actions"]) {
                    ScriptAction action;
                    action.type = static_cast<ScriptActionType>(actionData.value("type", static_cast<int>(ScriptActionType::SetVelocity)));
                    action.x = actionData.value("x", 0.0f);
                    action.y = actionData.value("y", 0.0f);
                    action.value = actionData.value("value", 0.0f);
                    action.target = actionData.value("target", "");
                    rule.actions.push_back(action);
                }
            }

            script->rules.push_back(rule);
        }
    }

    entity->addComponent(script, ComponentTypes::Script);
}

/**
 * @brief Loads a Text component from JSON onto an entity.
 * @param componentData The JSON data for the text component.
 * @param entity The entity receiving the component.
 * @param resourceManager The resource manager used to retrieve the font.
 */
void SceneLoader::loadText(const json &componentData, std::shared_ptr<Entity> entity, ResourceManager& resourceManager)
{
    std::string fontID = componentData.value("font", "");
    if(fontID.empty()) {
        std::cerr << "Text component missing font ID." << std::endl;
        return;
    }
    try {
        auto text = std::make_shared<Text>(resourceManager.getFont(fontID), fontID);
        text->characterSize = componentData.value("characterSize", 60);
        text->style = static_cast<sf::Text::Style>(componentData.value("style", static_cast<int>(sf::Text::Style::Regular)));
        text->linkGameValue = componentData.value("linkGameValue", false);
        text->positionRelativeToViewport = componentData.value("positionRelativeToViewport", false);
        text->linkedGameValue = static_cast<GameValue>(componentData.value("linkedGameValue", static_cast<int>(GameValue::PlayerHealth)));
        text->textString = componentData.value("textString", "new text component");
        text->fillColor.r = componentData.value("fillColorR", sf::Color::White.r);
        text->fillColor.g = componentData.value("fillColorG", sf::Color::White.g);
        text->fillColor.b = componentData.value("fillColorB", sf::Color::White.b);
        text->fillColor.a = componentData.value("fillColorA", sf::Color::White.a);
        text->applyProperties();
        entity->addComponent(text, ComponentTypes::Text);
    }
    catch(const std::exception& e) {
        std::cerr << "Failed to create text component: " << e.what() << std::endl;
    }
}

/**
 * @brief Loads a Switch component from JSON onto an entity.
 * @param componentData The JSON data for the switch component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadSwitch(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto switchComponent = std::make_shared<Switch>();
    switchComponent->activated = componentData.value("activated", false);
    switchComponent->activatedTextureID = componentData.value("activatedTextureID", "");
    switchComponent->deactivatedTextureID = componentData.value("deactivatedTextureID", "");
    entity->addComponent(switchComponent, ComponentTypes::Switch);
}

/**
 * @brief Loads a PowerUp component from JSON onto an entity.
 * @param componentData The JSON data for the power-up component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadPowerUp(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto powerUp = std::make_shared<PowerUp>();
    powerUp->velocityBoost = componentData.value("velocityBoost", powerUp->velocityBoost);
    powerUp->moveSpeedBoost = componentData.value("moveSpeedBoost", powerUp->moveSpeedBoost);
    powerUp->powerUpDuration = componentData.value("powerUpDuration", powerUp->powerUpDuration);
    powerUp->powerUpTimeLeft = componentData.value("powerUpTimeLeft", powerUp->powerUpTimeLeft);
    powerUp->powerUpType = static_cast<PowerUpType>(componentData.value("powerUpType", static_cast<int>(powerUp->powerUpType)));
    powerUp->selectionMethod = static_cast<SelectionMethod>(componentData.value("selectionMethod", static_cast<int>(powerUp->selectionMethod)));
    entity->addComponent(powerUp, ComponentTypes::PowerUp);
}

/**
 * @brief Loads a PopText component from JSON onto an entity.
 * @param componentData The JSON data for the pop text component.
 * @param entity The entity receiving the component.
 * @param resourceManager The resource manager used to retrieve the font.
 */
void SceneLoader::loadPopText(const json &componentData, std::shared_ptr<Entity> entity, ResourceManager& resourceManager)
{
    std::string fontID = componentData.value("font", ResourceManager::DefaultFontID);
    if (!resourceManager.hasFont(fontID)) {
        fontID = ResourceManager::DefaultFontID;
    }

    try {
        auto popText = std::make_shared<PopText>(resourceManager.getFont(fontID), fontID);
        popText->offset = readVec2(componentData, "offset", readVec2(componentData, "position", {0.0f, 0.0f}));
        popText->textString = componentData.value("textString", popText->textString);
        popText->size = componentData.value("characterSize", popText->size);
        popText->style = static_cast<sf::Text::Style>(componentData.value("style", static_cast<int>(popText->style)));
        popText->color.r = componentData.value("fillColorR", popText->color.r);
        popText->color.g = componentData.value("fillColorG", popText->color.g);
        popText->color.b = componentData.value("fillColorB", popText->color.b);
        popText->color.a = componentData.value("fillColorA", popText->color.a);
        popText->lifeTimeDuration = componentData.value("lifeTimeDuration", popText->lifeTimeDuration);
        popText->applyProperties();
        entity->addComponent(popText, ComponentTypes::PopText);
    }
    catch(const std::exception& e) {
        std::cerr << "Failed to create pop text component: " << e.what() << std::endl;
    }
}

/**
 * @brief Loads a Loot component from JSON onto an entity.
 * @param componentData The JSON data for the loot component.
 * @param entity The entity receiving the component.
 */
void SceneLoader::loadLoot(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto loot = std::make_shared<Loot>();
    loot->gold = componentData.value("gold", loot->gold);
    loot->score = componentData.value("score", loot->score);
    loot->experience = componentData.value("experience", loot->experience);
    entity->addComponent(loot, ComponentTypes::Loot);
}

/**
 * @brief Loads a Sound component from JSON onto an entity.
 * @param componentData The JSON data for the sound component.
 * @param entity The entity receiving the component.
 * @param resourceManager The resource manager used to retrieve the sound buffer.
 */
void SceneLoader::loadSound(const json &componentData, std::shared_ptr<Entity> entity, ResourceManager& resourceManager)
{
    std::string soundBufferID = componentData.value("soundBuffer", componentData.value("soundBufferID", ""));
    if (soundBufferID.empty()) {
        std::cerr << "Sound component missing sound buffer ID." << std::endl;
        return;
    }

    try {
        auto sound = std::make_shared<Sound>(resourceManager.getSoundBuffer(soundBufferID), soundBufferID);
        entity->addComponent(sound, ComponentTypes::Sound);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create sound component: " << e.what() << std::endl;
    }
}
