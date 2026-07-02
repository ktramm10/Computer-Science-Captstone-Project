#include "SceneSerializer.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

json SceneSerializer::writeVec2(const sf::Vector2f &vector) const
{
    return json::array({vector.x, vector.y});
}

void SceneSerializer::saveTextures(json &sceneData, const ResourceManager &resourceManager) const
{
    std::cout << "save textures method called" << std::endl;
    sceneData["textures"] = json::array();

    for (const auto& [id, path] : resourceManager.getTexturePaths()) {
        sceneData["textures"].push_back({ {"id", id}, {"path", path}});
    }
}

json SceneSerializer::saveEntity(const std::shared_ptr<Entity> &entity, const Scene& scene) const
{
    json entityData;
    entityData["name"] = entity->getName();
    entityData["isPlayer"] = entity == scene.getPlayerEntity();
    entityData["components"] = json::object();
    json& components = entityData["components"];

    saveTransformComponent(components, entity);
    saveSpriteComponent(components, entity);
    saveCollisionComponent(components, entity);
    savePhysicsComponent(components, entity);
    saveInputComponent(components, entity);
    saveHealthComponent(components, entity);
    saveInteractableComponent(components, entity);
    saveCollectableComponent(components, entity);

    return entityData;
}

void SceneSerializer::saveTransformComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Transform) != ComponentTypes::Transform) {
        return;
    }

    auto transform = entity->getComponent<Transform>();

    if (!transform) return;

    components["transform"] = {
        {"position", writeVec2(transform->position)}, 
        {"scale", writeVec2(transform->scale)}, 
        {"rotation", transform->rotation}
    };
}

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
        {"isTrigger", collision->isTrigger}
    };
}

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
        {"useGravity", physics->useGravity}
    };
}

void SceneSerializer::saveInputComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Input) != ComponentTypes::Input) {
        return;
    }
    auto input = entity->getComponent<Input>();
    if (!input) return;

    components["input"] = {
        {"moveSpeed", input->moveSpeed},
        {"jumpForce", input->jumpForce},
        {"canJump", input->canJump},
        {"canMove", input->canMove}
    };
}

void SceneSerializer::saveHealthComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Health) != ComponentTypes::Health) {
        return;
    }
    auto health = entity->getComponent<Health>();
    if (!health) return;

    components["health"] = {
        {"currentHealth", health->getCurrentHealth()},
        {"maxHealth", health->getMaxHealth()}
    };
}

void SceneSerializer::saveInteractableComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Interactable) != ComponentTypes::Interactable) {
        return;
    }
    auto interactable = entity->getComponent<Interactable>();
    if (!interactable) return;

    components["interactable"] = {
        {"interactionType", static_cast<int>(interactable->interactionType)}
    };
}

void SceneSerializer::saveCollectableComponent(json &components, const std::shared_ptr<Entity> &entity) const
{
    if ((entity->getComponentMask() & ComponentTypes::Collectable) != ComponentTypes::Collectable) {
        return;
    }
    auto collectable = entity->getComponent<Collectable>();
    if (!collectable) return;

    components["collectable"] = {
        {"collectableType", static_cast<int>(collectable->collectableType)},
        {"amount", collectable->amount}
    };
}

bool SceneSerializer::saveToFile(const std::string &filePath, const Scene &scene, const ResourceManager &resourceManager) const
{
    json sceneData;
    saveTextures(sceneData, resourceManager);
    sceneData["entities"] = json::array();
    for (const auto& entity : scene.getEntities()) {
        sceneData["entities"].push_back(saveEntity(entity, scene));
    }
    std::ofstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open scene file for saving: " << filePath << std::endl;
        return false;
    }
    file << sceneData.dump(4);
    return true;
}
