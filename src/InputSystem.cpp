#include "InputSystem.h"

void InputSystem::update(Scene& scene) {
    for (auto& entity : scene.getEntities()) {
        const auto componentMask = entity->getComponentMask();
        if ((componentMask & ComponentTypes::Input) != 0 && (componentMask & ComponentTypes::Physics) != 0) {
            auto input = entity->getComponent<Input>();
            auto physics = entity->getComponent<Physics>();
            // process input
            physics->velocity.x = 0.0f;

            if (!input->canMove) {
                continue;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
                physics->velocity.x = -input->moveSpeed;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
                physics->velocity.x = input->moveSpeed;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
                if (!physics->isGrounded) {
                    continue;
                }
                physics->velocity.y = -input->jumpForce;
                physics->isGrounded = false;
            }
        }
    }
}