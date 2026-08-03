#pragma once
#include "Component.h"
#include <filesystem>
#include <string>
#include <vector>

static const std::filesystem::path SCRIPTS_FILE_PATH = "../../src/json/scripts";

enum class ScriptEvent {
    OnStart, // 0
    OnUpdate, // 1
    OnCollision, // 2
    OnTimeout, // 3
    OnDestroyed, // 4
    OnCollisionWithPlayer, // 5
    OnSwitchActivate, // 6
    OnSwitchDeactivate, // 7
    OnPowerUpCollected, // 8
    OnPlayerDeath // 9
};

enum class ScriptActionType {
    RequestSceneLoad, // 0
    SetRotation, // 1
    AddRotation, // 2
    SetVelocity, // 3
    AddVelocityInRandomDirection, // 4
    SetVelocityInRandomDirection, // 5
    AddVelocity, // 6
    SetSpriteOrigin, // 7
    SetLocalSpritePosition, // 8
    StartTimer, // 9
    SetVelocityTowardsPlayer, // 10
    AddVelocityTowardsPlayer, // 11
    UnlockDoor, // 12
    SpawnEntity, // 13
    DisableEntity, // 14
    EnableEntity, // 15
    DestroyEntity, // 16
    SpawnPopText, // 17
    CollectLoot, // 18
    PlaySound, // 19
    StopSound // 20
    
};

struct ScriptAction {
    ScriptActionType type;
    float x = 0.0f;
    float y = 0.0f;
    float value = 0.0f;
    std::string target = "";
};

struct ScriptRule {
    ScriptEvent event;
    std::vector<ScriptAction> actions;
};

class Script : public Component {
    public:
    bool started = false;
    std::vector<ScriptRule> rules;
    std::string filePath;
    std::string fileName;
    static std::vector<std::filesystem::path> loadScriptsList(const std::filesystem::path &folder);

    Script() = default;
};