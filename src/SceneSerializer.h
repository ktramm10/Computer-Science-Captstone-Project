#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

#include "Scene.h"
#include "Entity.h"
#include "ResourceManager.h"

/**
 * @class SceneSerializer
 * @brief Serializes Scene entities, components, and global settings into JSON files.
 */
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
    void saveSoundComponent(json& components, const std::shared_ptr<Entity>& entity) const;

    public:
    /**
     * @brief Saves the entire scene to a JSON file.
     * @param filePath The path of the scene file to write.
     * @param scene The scene to serialize.
     * @param resourceManager The resource manager associated with the scene.
     * @return True if the scene was saved successfully, false otherwise.
     */
    bool saveToFile(const std::string& filePath, const Scene& scene, const ResourceManager& resourceManager) const;

};