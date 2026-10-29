#pragma once
#include "Component.h"
#include <iostream>

/**
 * @brief Enum representing game state values that can be displayed by a text component.
 */
enum class GameValue {
    PlayerHealth,
    Coins,
    Score,
    Experience
} ;

/**
 * @class Text
 * @brief Component that stores renderable text settings and optional game value binding.
 */
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

    /**
     * @brief Constructs a text component with a font and font resource ID.
     * @param font The font used by the SFML text object.
     * @param fontID The resource ID associated with the font.
     */
    Text(sf::Font& font, std::string fontID) {
        text = std::make_shared<sf::Text>(font, textString);
        this->fontID = fontID;
        applyProperties();
    }

    /**
     * @brief Appends a game value string to the base text string.
     * @param value The game value text to append.
     */
    void concatGameValue(const std::string& value) {
        if (!text) return;
        text->setString(textString + value);
    }

    /**
     * @brief Applies stored text properties to the wrapped SFML text object.
     */
    void applyProperties() {
        if (!text) return;
        text->setString(textString);
        text->setStyle(style);
        text->setCharacterSize(characterSize);
        text->setFillColor(fillColor);
    }
};