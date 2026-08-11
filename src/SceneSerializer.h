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
    json saveEntity(const std::shared_ptr<Entity>& entity, const Scene& scene) const;
    void saveTransformComponent(json& components, const std::shared_ptr<Entity>& entity, const Scene& scene) const;
    void saveCollisionComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveSpriteComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void savePhysicsComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveInputComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveHealthComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void savePickUpComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveContactDamageComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveAIComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveShooterComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveLifeTimeComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveSpawnerComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveDoorComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveKeyComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveScriptComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveTextComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveSwitchComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void savePowerUpComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void savePopTextComponent(json& components, const std::shared_ptr<Entity>& entity) const;
    void saveLootComponent(json& components, const std::shared_ptr<Entity>& entity) const;

    public:
    bool saveToFile(const std::string& filePath, const Scene& scene, const ResourceManager& resourceManager) const;

};