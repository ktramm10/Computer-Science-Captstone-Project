#include "SceneLoader.h"

#include <fstream>
#include <iostream>

bool SceneLoader::loadFromFile(const std::string &filePath, Scene &scene, ResourceManager &resourceManager)
{
    std::ifstream file(filePath);
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
    loadTextures(sceneData, resourceManager);
    if (!sceneData.contains("entities") || !sceneData["entities"].is_array()) {
        std::cerr << "Scene file has no entities array." << std::endl;
        return false;
    }

    for (const auto& entityData : sceneData["entities"]) {
        loadEntity(entityData, scene, resourceManager);
    }
    return true;
}

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

void SceneLoader::loadTextures(const json &sceneData, ResourceManager &resourceManager) const
{
    if (!sceneData.contains("textures") || !sceneData["textures"].is_array()) {
        return;
    }

    for (const auto& textureData : sceneData["textures"]) {
        std::string id = textureData.value("id", "");
        std::string path = textureData.value("path", "");

        if (id.empty() || path.empty()) {
            std::cerr << "Invalid texture entry." << std::endl;
            continue;
        }
        if (!resourceManager.loadTexture(id, path)) {
            std::cerr << "Failed to load texture: " << id << " from " << path << std::endl;
        }
    }
}

void SceneLoader::loadEntity(const json &entityData, Scene &scene, ResourceManager &resourceManager)
{
    std::string name = entityData.value("name", "entity");
    auto entity = scene.createEntity(name);

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
}

void SceneLoader::loadTransform(const json &componentData, std::shared_ptr<Entity> entity)
{
    sf::Vector2f position = readVec2(componentData, "position", {0.0f, 0.0f});
    sf::Vector2f scale = readVec2(componentData, "scale", {1.f, 1.f});
    float rotation = componentData.value("rotation", 0.0f);
    auto transform = std::make_shared<Transform>(position, scale, rotation);
    entity->addComponent(transform, ComponentTypes::Transform);
}

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

void SceneLoader::loadCollision(const json &componentData, std::shared_ptr<Entity> entity)
{
    sf::Vector2f size = readVec2(componentData, "size", {32.f, 32.f});
    sf::Vector2f offset = readVec2(componentData, "offset", {0.0f, 0.0f});
    bool isStatic = componentData.value("isStatic", false);
    bool isTriggered = componentData.value("isTriggered", false);

    auto collision = std::make_shared<Collision>(size, offset, isStatic, isTriggered);
    entity->addComponent(collision, ComponentTypes::Collision);
}

void SceneLoader::loadPhysics(const json &componentData, std::shared_ptr<Entity> entity)
{
    auto physics = std::make_shared<Physics>();
    physics->velocity = readVec2(componentData, "velocity", {0.0f, 0.0f});
    physics->acceleration = readVec2(componentData, "acceleration", {0.0f, 0.0f});
    physics->gravity = componentData.value("gravity", 150.0f);
    physics->isGrounded = componentData.value("isGrounded", false);
    physics->useGravity = componentData.value("useGravity", true);
    entity->addComponent(physics, ComponentTypes::Physics);
}

void SceneLoader::loadInput(const json &componentData, std::shared_ptr<Entity> entity)
{
    float moveSpeed = componentData.value("moveSpeed", 100.0f);
    float jumpForce = componentData.value("jumpForce", 300.0f);

    auto input = std::make_shared<Input>(moveSpeed, jumpForce);
    entity->addComponent(input, ComponentTypes::Input);
}

void SceneLoader::loadHealth(const json &componentData, std::shared_ptr<Entity> entity)
{
    float maxHealth = componentData.value("maxHealth", 100.0f);
    float currentHealth = componentData.value("currentHealth", maxHealth);

    auto health = std::make_shared<Health>(maxHealth, currentHealth);
    entity->addComponent(health, ComponentTypes::Health);
}
