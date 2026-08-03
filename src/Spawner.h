#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>
#include <string>
#include "Entity.h"


enum class SpawnMethod {
    Random,
    Fixed
};

static int idIndex = 1;

class Spawner : public Component {
    // 
    public:
    std::string baseName = "";
    SpawnMethod spawnMethod = SpawnMethod::Random;
    float spawnFrequency = 0.0f;
    float spawnTimeLeft = 0.0f;
    sf::Vector2f spawnPosition{0.f, 0.f};

    Spawner() = default;
    
};