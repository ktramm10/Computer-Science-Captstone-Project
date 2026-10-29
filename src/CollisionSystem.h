#pragma once

#include "Scene.h"
#include "Entity.h"
#include "ScriptSystem.h"
#include "AudioSystem.h"

/**
 * @class CollisionSystem
 * @brief Collision system class that handles collision detection and resolution between entities in a scene.
 * @details This class provides functionality to update the collision system for a given scene, handling collisions between entities and applying necessary corrections.
 *  It interacts with the SceneLoader, ResourceManager, and ScriptSystem to manage scene transitions, resource loading, and event handling related to collisions.
 */
class CollisionSystem {
    public:
    void update(Scene& scene, float dt, SceneLoader& sceneLoader, ResourceManager& resourceManager, ScriptSystem& scriptSystem, AudioSystem& audioSystem);

};