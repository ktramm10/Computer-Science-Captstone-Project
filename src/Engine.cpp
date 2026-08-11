#include "Engine.h"
#include <iostream>
#include "GameConfig.h"


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

Engine::Engine()
    : window(sf::VideoMode(static_cast<sf::Vector2u>(WINDOW_SIZE)), "AI Game Engine")
{
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to initialize ImGui-SFML." << std::endl;
    }
}

void Engine::run()
{
    std::cout << __cplusplus << std::endl;
    musicPlayer.setLooping(true);
    camera = sf::View(sf::FloatRect({0.f, 0.f}, {1920.f, 1080.f}));
    camera.setCenter(VIEWPORT_CENTER_POSITION);
    resourceManager.createDefaultTexture();
    resourceManager.loadTextureManifest("../../src/assets/textures.json");
    resourceManager.createDefaultFont();
    resourceManager.loadFontManifest("../../src/assets/fonts.json");
    resourceManager.loadSoundBufferManifest("../../src/assets/sounds.json");
    resourceManager.loadMusicManifest("../../src/assets/music.json");
    scene.initialize(resourceManager);

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();
        float dt = deltaTime.asSeconds();
        
        processEvents();
        ImGui::SFML::Update(window, deltaTime);
        if (isPlaying) {
            inputSystem.update(scene, dt, window, resourceManager);
            timerManager.update(scene, dt, scriptSystem);
            scriptSystem.update(scene, dt);
            physicsSystem.update(scene, dt);
            collisionSystem.update(scene, dt, sceneLoader, resourceManager, scriptSystem);
            if (scene.hasRequestedSceneLoad()) {
                if (sceneLoader.loadFromFile(scene.consumeRequestedSceneLoad(), scene, resourceManager)) {
                    camera.setCenter(VIEWPORT_CENTER_POSITION);
                }
            }
        }
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

int main() {
    Engine engine;
    engine.run();

    return 0;
}
