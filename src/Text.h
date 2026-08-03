#pragma once
#include "Component.h"
#include <iostream>
enum class GameValue {
    PlayerHealth,
    Coins,
    Score,
    Experience
} ;

class Text : public Component {
    public:
    std::shared_ptr<sf::Text> text;
    std::string fontID = "";
    sf::Text::Style style = sf::Text::Style::Regular;
    bool linkGameValue = false;
    bool positionRelativeToViewport = false;
    GameValue linkedGameValue = GameValue::PlayerHealth;
    unsigned int characterSize = 60;
    std::string textString = "new text component";
    sf::Color fillColor = sf::Color::White;
    Text(sf::Font& font, std::string fontID) {
        text = std::make_shared<sf::Text>(font, textString);
        this->fontID = fontID;
        applyProperties();
    }

    void concatGameValue(const std::string& value) {
        if (!text) return;
        text->setString(textString + value);
    }

    void applyProperties() {
        if (!text) return;
        text->setString(textString);
        text->setStyle(style);
        text->setCharacterSize(characterSize);
        text->setFillColor(fillColor);
    }
};