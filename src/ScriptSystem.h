#pragma once
#include <memory>
#include "Script.h"

class Entity;
class Scene;

class ScriptSystem {
    private:
    void runEvent(Scene& scene, std::shared_ptr<Entity> entity, Script& script, ScriptEvent event, float dt);

    public:
    void update(Scene& scene, float dt);
    void OnTimeout(Scene& scene, float dt, std::shared_ptr<Entity> entity);
    void onCollision(Scene &scene, float dt, std::shared_ptr<Entity> EntityA, std::shared_ptr<Entity> EntityB);
    void onDestroyed(Scene& scene, float dt, std::shared_ptr<Entity> entity);
    void onSwitchActivate(Scene& scene, float dt, std::shared_ptr<Entity> entity);
    void onSwitchDeactivate(Scene& scene, float dt, std::shared_ptr<Entity> entity);
    void OnPowerUpCollected(Scene& scene, float dt, std::shared_ptr<Entity> entity);
    void OnPlayerDeath(Scene& scene, float dt);
    void dispatchDestroyedEvents(Scene& scene, float dt);
};