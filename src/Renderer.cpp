#include "Renderer.h"

void Renderer::render(const Scene& scene, sf::RenderWindow& window)
{
    for (const auto& entity : scene.getEntities()) {
        if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Sprite)) {
            auto transform = entity->getComponent<Transform>();
            auto sprite = entity->getComponent<Sprite>();
            if (transform && sprite) {
                sprite->sprite->setPosition(sf::Vector2f(transform->position.x, transform->position.y));
                sprite->sprite->setScale(sf::Vector2f(transform->scale.x, transform->scale.y));
                sprite->sprite->setRotation(sf::degrees(transform->rotation));

                window.draw(*sprite->sprite);
            }
        }
    }
}