#include "Engine.h"
#include <iostream>

Engine::Engine() : window (sf::VideoMode({1920, 1080}), "AI Game Engine") {}

void Engine::run()
{
    bool loaded = sceneLoader.loadFromFile("..\\..\\src\\json\\saved_scene.json", scene, resourceManager);
    if (!loaded) {
        std::cerr << "Failed to load the scene." << std::endl;
    }
    

    scene.initialize();
    scene.debugPrintEntities(); // Debug: Print entities after loading the scene

    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        processEvents();
        inputSystem.update(scene);
        physicsSystem.update(scene, dt);
        collisionSystem.update(scene);
        scene.cleanupDestroyedEntities();
        window.clear(sf::Color::Black);
        renderer.render(scene, window);
        window.display();
    }
}

void Engine::processEvents()
{
    
    while (auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        
        else if (event->is<sf::Event::KeyPressed>()) {
            const auto* keyEvent = event->getIf<sf::Event::KeyPressed>();
            if (keyEvent->code == sf::Keyboard::Key::F5) {
                sceneSerializer.saveToFile("..\\..\\src\\json\\saved_scene.json", scene, resourceManager);
            }
        }
    }
}

int main() {
    Engine engine;
    engine.run();

    return 0;
}
