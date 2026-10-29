#pragma once


#include <SFML/Audio.hpp>
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
#include "AudioSystem.h"

/**
 * @class Engine
 * @brief Engine class that manages the main game loop, event processing, and rendering.
 * @details This class encapsulates the core functionality of the game engine, including managing the SFML window, handling events, updating systems, and rendering the scene and UI.
 */
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
    AudioSystem audioSystem;
    EditorUI editorUI;
    sf::View camera;
    sf::Music musicPlayer;
    void processEvents();
    

    public:
    std::vector<std::filesystem::path> getSceneFiles(const std::filesystem::path& folder);
    ~Engine();
    Engine();
    void run();
    bool isPlaying = false;
    
};