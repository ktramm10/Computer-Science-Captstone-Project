#pragma once

#include "Component.h"
#include "cmath"



/**
 * @class Transform
 * @brief Component that stores world position, local position, rotation, and scale for an entity.
 */
class Transform : public Component
{
    private:
    sf::Vector2f position;
    sf::Vector2f localPosition{0.f, 0.f}; 
    float rotation = 0.0f;
    sf::Vector2f scale = sf::Vector2f(1.0f, 1.0f);

    public:
    /**
     * @brief Constructs a transform with default position, rotation, and scale values.
     */
    Transform() = default;

    /**
     * @brief Constructs a transform with explicit world position, scale, and rotation.
     * @param position The initial world position.
     * @param scale The initial scale.
     * @param rotation The initial rotation in degrees.
     */
    Transform(sf::Vector2f position, sf::Vector2f scale, float rotation )
        : position(position), scale(scale), rotation(rotation)  {}

    /**
     * @brief Retrieves the transform's world position.
     * @return The world position vector.
     */
    sf::Vector2f getWorldPosition() const { return position; }

    /**
     * @brief Retrieves the transform's local position relative to its parent.
     * @return The local position vector.
     */
    sf::Vector2f getLocalPosition() const { return localPosition; }

    /**
     * @brief Retrieves the transform's rotation.
     * @return The rotation value in degrees.
     */
    float getRotation() const { return rotation; }

    /**
     * @brief Retrieves the transform's scale.
     * @return The scale vector.
     */
    sf::Vector2f getScale() const { return scale; }

    /**
     * @brief Sets the transform's world position.
     * @param position The new world position.
     */
    void setWorldPosition(sf::Vector2f position) {
        this->position = position;
    }

    /**
     * @brief Moves the transform's world position by a delta.
     * @param delta The amount to add to the world position.
     */
    void moveWorldPosition(sf::Vector2f delta) {
        position += delta;
    }

    /**
     * @brief Sets the transform's local position.
     * @param position The new local position.
     */
    void setLocalPosition(sf::Vector2f position) {
        localPosition = position;
    }

    /**
     * @brief Sets the transform's rotation.
     * @param rotation The new rotation in degrees.
     */
    void setRotation(float rotation) {
        this->rotation = rotation;
    }

    /**
     * @brief Sets the transform's x scale.
     * @param scaleX The new x scale value.
     */
    void setScaleX(float scaleX) {
        scale.x = scaleX;
    }

    /**
     * @brief Sets the transform's y scale.
     * @param scaleY The new y scale value.
     */
    void setScaleY(float scaleY) {
        scale.y = scaleY;
    }

    /**
     * @brief Sets the transform's scale vector.
     * @param scale The new scale vector.
     */
    void setScale(sf::Vector2f scale) {
        this->scale = scale;
    }


};