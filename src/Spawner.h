#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>
#include <string>
#include "Entity.h"


/**
 * @brief Enum representing the method used to choose spawn positions.
 */
enum class SpawnMethod {
    Random,
    Fixed
};

/**
 * @brief Global suffix index used when naming spawned entities.
 */
static int idIndex = 1;

/**
 * @class Spawner
 * @brief Component that stores timed entity spawning configuration.
 */
class Spawner : public Component {
    // 
    public:
    std::string baseName = "";
    SpawnMethod spawnMethod = SpawnMethod::Random;
    float spawnFrequency = 0.0f;
    float spawnTimeLeft = 0.0f;
    sf::Vector2f spawnPosition{0.f, 0.f};

    /**
     * @brief Constructs a spawner component with default spawn settings.
     */
    Spawner() = default;
    
};