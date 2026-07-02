#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

#include "Scene.h"
#include "Entity.h"
#include "ResourceManager.h"

class SceneSerializer {
    private:
    using json = nlohmann::json;
    json writeVec2(const sf::Vector2f& vector) const;
    void saveTextures(json& sceneData, const ResourceManager& resourceManager) const;
    json saveEntity(const std::shared_ptr<Entity>& entity, const Scene& scene) const;
    void saveTransformComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveCollisionComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveSpriteComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void savePhysicsComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveInputComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveHealthComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveInteractableComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveCollectableComponent(json& components, const std::shared_ptr<Entity>& entity) const;

    public:
    bool saveToFile(const std::string& filePath, const Scene& scene, const ResourceManager& resourceManager) const;

};