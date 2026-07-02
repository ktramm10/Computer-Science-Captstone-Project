#pragma once

#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"
#include "InputSystem.h"
#include "ResourceManager.h"
#include "SceneLoader.h"
#include "SceneSerializer.h"

class Engine {
    private:
    sf::RenderWindow window;
    ResourceManager resourceManager;
    SceneLoader sceneLoader;
    SceneSerializer sceneSerializer;
    Scene scene;
    Renderer renderer;
    PhysicsSystem physicsSystem;
    CollisionSystem collisionSystem;
    InputSystem inputSystem;
    void processEvents();

    public:
    Engine();
    void run();
    
};