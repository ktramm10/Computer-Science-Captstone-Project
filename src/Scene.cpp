#include "Scene.h"
std::shared_ptr<Entity> Scene::createEntity(const std::string& name) {
    auto entity = std::make_shared<Entity>(nextEntityID++, name);
    entities.push_back(entity);
    return entity;
}

const std::vector<std::shared_ptr<Entity>>& Scene::getEntities() const {
    return entities;
}

void Scene::destroyEntity(std::shared_ptr<Entity> entity) {
    entity->destroy();
}

void Scene::cleanupDestroyedEntities() {
    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [](const std::shared_ptr<Entity>& entity) {
            return !entity->isAlive();
        }), entities.end());
}
void Scene::initialize(){}
void Scene::clear() {
    entities.clear();
    nextEntityID = 1;
}
