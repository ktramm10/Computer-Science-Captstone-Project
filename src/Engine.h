#pragma once


#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"
#include "InputSystem.h"
#include "ScriptSystem.h"
#include "ResourceManager.h"
#include "SceneLoader.h"
#include "SceneSerializer.h"
#include "EditorUI.h"
#include "TimerManager.h"

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
    ScriptSystem scriptSystem;
    TimerManager timerManager;
    EditorUI editorUI;
    sf::View camera;
    void processEvents();
    

    public:
    std::vector<std::filesystem::path> getSceneFiles(const std::filesystem::path& folder);
    ~Engine();
    Engine();
    void run();
    bool isPlaying = false;
    
};