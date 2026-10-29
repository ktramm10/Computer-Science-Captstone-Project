#pragma once
#include "Component.h"
#include "ProjectPaths.h"
#include <filesystem>
#include <string>
#include <vector>

static const std::filesystem::path SCRIPTS_FILE_PATH = resolveProjectPath("src/json/scripts");

/**
 * @brief Enum representing events that can trigger script rules.
 */
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

/**
 * @brief Enum representing actions that can be performed by script rules.
 */
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
    StopSound, // 20
    ReloadCurrentScene // 21
    
};

/**
 * @brief Data describing a single script action and its optional numeric or target arguments.
 */
struct ScriptAction {
    ScriptActionType type;
    float x = 0.0f;
    float y = 0.0f;
    float value = 0.0f;
    std::string target = "";
};

/**
 * @brief Data describing a script event and the actions that run when it is triggered.
 */
struct ScriptRule {
    ScriptEvent event;
    std::vector<ScriptAction> actions;
};

/**
 * @class Script
 * @brief Component that stores event-driven script rules for an entity.
 */
class Script : public Component {
    public:
    bool started = false;
    std::vector<ScriptRule> rules;
    std::string filePath;
    std::string fileName;

    /**
     * @brief Loads the list of available script JSON files from a folder.
     * @param folder The folder to search for script files.
     * @return A vector containing the paths of all JSON script files found in the folder.
     */
    static std::vector<std::filesystem::path> loadScriptsList(const std::filesystem::path &folder);

    /**
     * @brief Constructs an empty script component.
     */
    Script() = default;
};