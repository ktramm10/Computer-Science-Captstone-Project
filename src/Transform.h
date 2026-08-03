#pragma once

#include "Component.h"
#include "cmath"



class Transform : public Component
{
    private:
    sf::Vector2f position;
    sf::Vector2f localPosition{0.f, 0.f}; 
    float rotation = 0.0f;
    sf::Vector2f scale = sf::Vector2f(1.0f, 1.0f);

    public:
    Transform() = default;
    Transform(sf::Vector2f position, sf::Vector2f scale, float rotation )
        : position(position), scale(scale), rotation(rotation)  {}

    sf::Vector2f getWorldPosition() const { return position; }
    sf::Vector2f getLocalPosition() const { return localPosition; }
    float getRotation() const { return rotation; }
    sf::Vector2f getScale() const { return scale; }
    void setWorldPosition(sf::Vector2f position) {
        this->position = position;
    }
    void moveWorldPosition(sf::Vector2f delta) {
        position += delta;
    }
    void setLocalPosition(sf::Vector2f position) {
        localPosition = position;
    }
    void setRotation(float rotation) {
        this->rotation = rotation;
    }
    void setScaleX(float scaleX) {
        scale.x = scaleX;
    }
    void setScaleY(float scaleY) {
        scale.y = scaleY;
    }
    void setScale(sf::Vector2f scale) {
        this->scale = scale;
    }


};