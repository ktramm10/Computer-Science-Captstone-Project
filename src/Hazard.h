#pragma once

#include "Component.h"
#include "Entity.h"
#include "Scene.h"
#include "ScriptSystem.h"
/**
 * @enum class representing different types of hazards that can affect entities in the game.
 */
enum class HazardType {
    Damage,
    Slow,  
    Burn
};

/**
 * @class Hazard
 * @brief Hazard component class that inherits from Component.
 * @details This class represents the hazard properties of an entity in a game. It contains various properties related to hazards.
 */
class Hazard : public Component {
    public:
    Hazard() = default;
    HazardType hazardType = HazardType::Damage;
    Hazard(HazardType hazardType) : hazardType(hazardType) {}
    void applyHazard(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> hazardEntity, Scene& scene, ScriptSystem& scriptSystem, float dt);
};