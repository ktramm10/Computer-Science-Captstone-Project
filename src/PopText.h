#pragma once
#include "Component.h"
#include "Text.h"
#include "LifeTime.h"
#include "Transform.h"
class PopText : public Component {
public:
    std::shared_ptr<Text> text;
    std::shared_ptr<LifeTime> lifeTime;
    std::shared_ptr<Transform> transform;
    

    sf::Vector2f offset;
    std::string textString;
    unsigned int size;
    sf::Color color;
    sf::Text::Style style;
    std::string fontID;
    
    float lifeTimeDuration = 0.0f;

    PopText(sf::Font& font, const std::string& fontID) {
        this->fontID = fontID;
        text = std::make_shared<Text>(font, fontID);
        lifeTime = std::make_shared<LifeTime>();
        transform = std::make_shared<Transform>();
        textString = text->textString;
        size = text->characterSize;
        color = text->fillColor;
        style = text->style;
    }

    void applyProperties() {
        if (text) {
            text->fontID = fontID;
            text->textString = textString;
            text->characterSize = size;
            text->fillColor = color;
            text->style = style;
            text->applyProperties();
        }
        if (lifeTime) {
            lifeTime->lifeTimeDuration = lifeTimeDuration;
        }
        if (transform) {
            sf::Vector2f worldPosition = offset;
            if (auto parent = getParent()) {
                if (auto parentTransform = parent->getComponent<Transform>()) {
                    worldPosition += parentTransform->getWorldPosition();
                }
            }
            transform->setWorldPosition(worldPosition);
        }
    }

};