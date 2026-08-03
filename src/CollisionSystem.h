#pragma once

#include "Scene.h"
#include "Entity.h"
#include "ScriptSystem.h"

class CollisionSystem {
    public:
    void update(Scene& scene, float dt, SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem);

};