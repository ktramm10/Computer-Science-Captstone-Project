#include "Engine.h"
#include <iostream>

Engine::Engine() : window (sf::VideoMode({1920, 1080}), "AI Game Engine") {}

void Engine::run()
{
    bool loaded = sceneLoader.loadFromFile("C:\\Development\\Computer Science Captstone Project\\src\\json\\test.json", scene, resourceManager);
    if (!loaded) {
        std::cerr << "Failed to load the scene." << std::endl;
    }

 // resourceManager.loadTexture("red_square", "C:\\Development\\Computer Science Captstone Project\\src\\images\\RedSpriteTest32x32.png");
 // auto player1 = scene.createEntity("Player");
 // player1->addComponent(std::make_shared<Transform>(), ComponentTypes::Transform);
 // player1->getComponent<Transform>()->position = sf::Vector2f(100.0f, 100.0f);
 // player1->addComponent(std::make_shared<Physics>(), ComponentTypes::Physics);
 // player1->addComponent(std::make_shared<Input>(), ComponentTypes::Input);
 // player1->addComponent(std::make_shared<Sprite>(resourceManager.getTexture("red_square")), ComponentTypes::Sprite);
 // player1->addComponent(std::make_shared<Collision>(sf::Vector2f(32.0f, 32.0f)), ComponentTypes::Collision);
 // auto player2 = scene.createEntity("Player2");
 // player2->addComponent(std::make_shared<Transform>(), ComponentTypes::Transform);
 // player2->getComponent<Transform>()->position = sf::Vector2f(1000.0f, 100.0f);
 // player2->addComponent(std::make_shared<Physics>(), ComponentTypes::Physics);
 // player2->addComponent(std::make_shared<Sprite>(resourceManager.getTexture("red_square")), ComponentTypes::Sprite);
 // player2->addComponent(std::make_shared<Collision>(sf::Vector2f(32.0f, 32.0f)), ComponentTypes::Collision);
 // auto platform = scene.createEntity("platform");
 // platform->addComponent(std::make_shared<Transform>(), ComponentTypes::Transform);
 // platform->getComponent<Transform>()->position = sf::Vector2f(0.0f, 1000.0f);
 // platform->addComponent(std::make_shared<Collision>(sf::Vector2f(1920.0f, 64.0f)), ComponentTypes::Collision);
 // platform->getComponent<Collision>()->isStatic = true;
    

    scene.initialize();

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
                sceneSerializer.saveToFile("C:\\Development\\Computer Science Captstone Project\\src\\json\\saved_scene.json", scene, resourceManager);
            }
        }
    }
}

int main() {
    Engine engine;
    engine.run();

    return 0;
}
