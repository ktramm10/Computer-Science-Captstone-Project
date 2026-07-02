#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

#include "Scene.h"
#include "ResourceManager.h"
#include "Entity.h"

class SceneLoader {
    private:
    using json = nlohmann::json;

    sf::Vector2f readVec2(const json& object, const std::string& key, const sf::Vector2f& defaultValue) const;
    void loadTextures(const json& sceneData, ResourceManager& resourceManager) const;
    void loadEntity(const json& entityData, Scene& scene, ResourceManager& resourceManager);
    void loadTransform(const json& componentData, std::shared_ptr<Entity> entity);
    void loadSprite(const json& componentData, std::shared_ptr<Entity> entity, ResourceManager& ResourceManager);
    void loadCollision(const json& componentData, std::shared_ptr<Entity> entity);
    void loadPhysics(const json& componentData, std::shared_ptr<Entity> entity);
    void loadInput(const json& componentData, std::shared_ptr<Entity> entity);
    void loadHealth(const json& componentData, std::shared_ptr<Entity> entity);
    void loadInteractable(const json& componentData, std::shared_ptr<Entity> entity);
    void loadCollectable(const json& componentData, std::shared_ptr<Entity> entity);

    public:
    bool loadFromFile(const std::string& filePath, Scene& scene, ResourceManager& ResourceManager);
};
