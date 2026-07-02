#pragma once

#include "Component.h"
#include <SFML/Graphics.hpp>

class Collision : public Component
{
   public:
   sf::Vector2f size{32.f, 32.f};
   sf::Vector2f offset{0.f, 0.f};
   bool isStatic = false;
   bool isTrigger = false;

   Collision() = default;

   Collision(const sf::Vector2f& size, const sf::Vector2f& offset = {0.f, 0.f}, bool isStatic = false, bool isTrigger = false)
       : size(size), offset(offset), isStatic(isStatic), isTrigger(isTrigger) {}

};