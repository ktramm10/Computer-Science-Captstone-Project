#pragma once

#include <string>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

class Entity;
class ResourceManager;
class Scene;

class SceneLoader {
    private:
    using json = nlohmann::json;

    sf::Vector2f readVec2(const json& object, const std::string& key, const sf::Vector2f& defaultValue) const;
    void loadEntity(const json& entityData, Scene& scene, ResourceManager& resourceManager);
    void loadTransform(const json& componentData, std::shared_ptr<Entity> entity);
    void loadSprite(const json& componentData, std::shared_ptr<Entity> entity, ResourceManager& ResourceManager);
    void loadCollision(const json& componentData, std::shared_ptr<Entity> entity);
    void loadPhysics(const json& componentData, std::shared_ptr<Entity> entity);
    void loadInput(const json& componentData, std::shared_ptr<Entity> entity);
    void loadHealth(const json& componentData, std::shared_ptr<Entity> entity);
    void loadPickUp(const json& componentData, std::shared_ptr<Entity> entity);
    void loadContactDamage(const json& componentData, std::shared_ptr<Entity> entity);
    void loadAI(const json& componentData, std::shared_ptr<Entity> entity);
    void loadShooter(const json& componentData, std::shared_ptr<Entity> entity);
    void loadLifeTime(const json& componentData, std::shared_ptr<Entity> entity);
    void loadSpawner(const json& componentData, std::shared_ptr<Entity> entity, Scene& scene);
    void loadDoor(const json& componentData, std::shared_ptr<Entity> entity);
    void loadKey(const json& componentData, std::shared_ptr<Entity> entity);
    void loadScript(const json& componentData, std::shared_ptr<Entity> entity);
    void loadText(const json& componentData, std::shared_ptr<Entity> entity, ResourceManager& resourceManager);
    void loadSwitch(const json& componentData, std::shared_ptr<Entity> entity);
    void loadPowerUp(const json& componentData, std::shared_ptr<Entity> entity);
    void loadPopText(const json& componentData, std::shared_ptr<Entity> entity, ResourceManager& resourceManager);
    void loadLoot(const json& componentData, std::shared_ptr<Entity> entity);

    public:
    bool loadFromFile(const std::string& filePath, Scene& scene, ResourceManager& ResourceManager);
};
