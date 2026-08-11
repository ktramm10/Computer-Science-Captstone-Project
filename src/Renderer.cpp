#include "Renderer.h"

void Renderer::render(const Scene& scene, sf::RenderWindow& window)
{
    for (const auto& entity : scene.getEntities()) {
        if (entity->isBlueprint()) {
            continue;
        }
        if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Sprite)) {
            auto transform = entity->getComponent<Transform>();
            auto sprite = entity->getComponent<Sprite>();
            if (transform && sprite) {
                sf::Vector2f renderPosition = scene.getWorldPosition(entity);
                if (!entity->getParentEntity().lock()) {
                    renderPosition += transform->getLocalPosition();
                }
                sprite->sprite->setPosition(renderPosition);
                sprite->sprite->setScale(sf::Vector2f(transform->getScale().x, transform->getScale().y));
                sprite->sprite->setRotation(sf::degrees(transform->getRotation()));

                window.draw(*sprite->sprite);
            }
        }
        if (entity->getComponentMask() & (ComponentTypes::Transform | ComponentTypes::Text)) {
            auto transform = entity->getComponent<Transform>();
            auto text = entity->getComponent<Text>();
            if (transform && text && text->text) {
                if (text->positionRelativeToViewport) {
                    text->text->setPosition(window.getView().getCenter() + transform->getLocalPosition());
                } else {
                    sf::Vector2f renderPosition = scene.getWorldPosition(entity);
                    if (!entity->getParentEntity().lock()) {
                        renderPosition += transform->getLocalPosition();
                    }
                    text->text->setPosition(renderPosition);
                }
                text->text->setScale(transform->getScale());
                text->text->setRotation(sf::degrees(transform->getRotation()));

                window.draw(*text->text);
            }
        }
    }
}