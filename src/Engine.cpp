#include "Engine.h"
#include <iostream>
#include "GameConfig.h"
#include "ProjectPaths.h"

/**
 * @brief Anonymous namespace for internal helper functions.
 */
namespace {
    void handleViewportMovement(Engine* engine, Scene& scene, float dt, sf::RenderWindow& window, sf::View& camera, EditorUI& editorUI) {
        bool isEditing = !engine->isPlaying && editorUI.getIsSceneLoaded();
        if (isEditing) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                camera.move({-VIEWPORT_CAMERA_SPEED * dt, 0.0f});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                camera.move({VIEWPORT_CAMERA_SPEED * dt, 0.0f});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                camera.move({0.0f, -VIEWPORT_CAMERA_SPEED * dt});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                camera.move({0.0f, VIEWPORT_CAMERA_SPEED * dt});
            }
            
        }
        bool isPlaying = engine->isPlaying && editorUI.getIsSceneLoaded();
        if (isPlaying) {
            bool shouldFollowPlayer = scene.getPlayerEntity() && !scene.getPlayerEntity()->isBlueprint() && VIEWPORT_FOLLOW_PLAYER_CHARACTER;
            if (shouldFollowPlayer) {
                auto transform = scene.getPlayerEntity()->getComponent<Transform>();
                if (transform) {
                    camera.setCenter(scene.getWorldPosition(scene.getPlayerEntity()) + VIEWPORT_CAMERA_OFFSET);
                }
            }
        }
        VIEWPORT_CENTER_POSITION = camera.getCenter();
        window.setView(camera);
    }
}

Engine::~Engine()
{
    ImGui::SFML::Shutdown();
}
/**
 * @brief Constructs the Engine object and initializes the SFML window and ImGui.
 */
Engine::Engine()
    : window(sf::VideoMode(static_cast<sf::Vector2u>(WINDOW_SIZE)), "AI Game Engine")
{
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to initialize ImGui-SFML." << std::endl;
    }
}
/**
 * @brief Runs the main game loop, handling events, updating systems, and rendering the scene and UI.
 */
void Engine::run()
{
    std::cout << __cplusplus << std::endl;
    musicPlayer.setLooping(true);
    camera = sf::View(sf::FloatRect({0.f, 0.f}, {1920.f, 1080.f}));
    camera.setCenter(VIEWPORT_CENTER_POSITION);
    resourceManager.createDefaultTexture();
    resourceManager.loadTextureManifest(resolveProjectPath("src/assets/textures.json").string());
    resourceManager.createDefaultFont();
    resourceManager.loadFontManifest(resolveProjectPath("src/assets/fonts.json").string());
    resourceManager.loadSoundBufferManifest(resolveProjectPath("src/assets/sounds.json").string());
    resourceManager.loadMusicManifest(resolveProjectPath("src/assets/music.json").string());
    scene.initialize(resourceManager);

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();
        float dt = deltaTime.asSeconds();
        
        processEvents();
        ImGui::SFML::Update(window, deltaTime);
        if (isPlaying) {
            inputSystem.update(scene, dt, window, resourceManager, audioSystem);
            timerManager.update(scene, dt, scriptSystem);
            scriptSystem.update(scene, dt);
            physicsSystem.update(scene, dt);
            collisionSystem.update(scene, dt, sceneLoader, resourceManager, scriptSystem, audioSystem);
            if (scene.hasRequestedSceneLoad()) {
                if (sceneLoader.loadFromFile(scene.consumeRequestedSceneLoad(), scene, resourceManager)) {
                    camera.setCenter(VIEWPORT_CENTER_POSITION);
                }
            }
        }
        audioSystem.update();
        handleViewportMovement(this, scene, dt, window, camera, editorUI);
        if (isPlaying) {
            scriptSystem.dispatchDestroyedEvents(scene, dt);
        }
        scene.cleanupDestroyedEntities();
        window.clear(scene.backgroundColor);
        renderer.render(scene, window);
        editorUI.buildUI(scene, resourceManager, sceneSerializer, sceneLoader, isPlaying, camera, musicPlayer); // Build the ImGui UI for the editor
        editorUI.handleDragMove(window, isPlaying);
        ImGui::SFML::Render(window);
        window.display();
    }
}
/**
 * @brief Processes SFML window events, including closing the window and passing events to ImGui.
 */
void Engine::processEvents()
{
    
    while (auto event = window.pollEvent())
    {
        ImGui::SFML::ProcessEvent(window, *event);

        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
    }
}
/**
 * @brief The main entry point of the application, creating an Engine instance and running it.
 */
int main() {
    Engine engine;
    engine.run();

    return 0;
}
