#pragma once

#include "Component.h"
#include "Entity.h"
#include "Scene.h"
#include "ScriptSystem.h"

enum class HazardType {
    Damage,
    Slow,  
    Burn
};

class Hazard : public Component {
    public:
    Hazard() = default;
    HazardType hazardType = HazardType::Damage;
    Hazard(HazardType hazardType) : hazardType(hazardType) {}
    void applyHazard(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> hazardEntity, Scene& scene, ScriptSystem& scriptSystem, float dt);
};