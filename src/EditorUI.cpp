#include "EditorUI.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

namespace {   

std::string getJsonScriptAsString(std::string filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file -> " << filePath << std::endl;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeStringIntoJsonFile(std::string content, std::string filePath) {
    std::ofstream file(filePath);
    if (!file) {
        std::cerr << "Could not open file -> " << filePath << std::endl;
        return;
    }
    file << content;
}

void loadScriptFromJson(const nlohmann::json& data, Script& script) {
    script.started = data.value("started", false);
    script.filePath = data.value("filePath", script.filePath);
    script.fileName = data.value("fileName", script.fileName);
    script.rules.clear();

    const nlohmann::json* rulesData = nullptr;
    if (data.is_array()) {
        rulesData = &data;
    }
    else if (data.contains("rules") && data["rules"].is_array()) {
        rulesData = &data["rules"];
    }

    if (!rulesData) {
        return;
    }

    for (const auto& ruleData : *rulesData) {
        ScriptRule rule;
        rule.event = static_cast<ScriptEvent>(ruleData.value("event", static_cast<int>(ScriptEvent::OnUpdate)));

        if (ruleData.contains("actions") && ruleData["actions"].is_array()) {
            for (const auto& actionData : ruleData["actions"]) {
                ScriptAction action;
                action.type = static_cast<ScriptActionType>(actionData.value("type", static_cast<int>(ScriptActionType::SetVelocity)));
                action.x = actionData.value("x", 0.0f);
                action.y = actionData.value("y", 0.0f);
                action.value = actionData.value("value", 0.0f);
                action.target = actionData.value("target", "");
                rule.actions.push_back(action);
            }
        }

        script.rules.push_back(rule);
    }
}

std::string getDuplicateBaseName(const std::string& entityName)
{
    size_t separatorPosition = entityName.find_last_of('_');
    if (separatorPosition == std::string::npos || separatorPosition == entityName.size() - 1) {
        return entityName;
    }

    for (size_t index = separatorPosition + 1; index < entityName.size(); ++index) {
        if (!std::isdigit(static_cast<unsigned char>(entityName[index]))) {
            return entityName;
        }
    }

    return entityName.substr(0, separatorPosition);
}

std::string makeDuplicateEntityName(const Scene& scene, const std::string& entityName)
{
    std::string baseName = getDuplicateBaseName(entityName);
    int suffix = 1;
    while (true) {
        std::string candidateName = baseName + "_" + std::to_string(suffix);
        bool nameExists = false;
        for (const auto& entity : scene.getEntities()) {
            if (entity->getName() == candidateName) {
                nameExists = true;
                break;
            }
        }

        if (!nameExists) {
            return candidateName;
        }
        ++suffix;
    }
}

const char* getFactionLabel(Faction faction)
{
    switch (faction) {
    case Faction::Player:
        return "Player";
    case Faction::Enemy:
        return "Enemy";
    case Faction::Neutral:
    default:
        return "Neutral";
    }
}

void drawFactionCombo(const char* label, Faction& faction)
{
    if (ImGui::BeginCombo(label, getFactionLabel(faction))) {
        if (ImGui::Selectable("Neutral", faction == Faction::Neutral)) faction = Faction::Neutral;
        if (ImGui::Selectable("Player", faction == Faction::Player)) faction = Faction::Player;
        if (ImGui::Selectable("Enemy", faction == Faction::Enemy)) faction = Faction::Enemy;
        ImGui::EndCombo();
    }
}

const char* getPickUpTypeLabel(PickUpType pickUpType)
{
    switch (pickUpType) {
    case PickUpType::Health:
        return "Health";
    case PickUpType::Ammo:
        return "Ammo";
    case PickUpType::Coin:
    default:
        return "Coin";
    }
}

void drawPickUpTypeCombo(const char* label, PickUpType& pickUpType)
{
    if (ImGui::BeginCombo(label, getPickUpTypeLabel(pickUpType))) {
        if (ImGui::Selectable("Coin", pickUpType == PickUpType::Coin)) pickUpType = PickUpType::Coin;
        if (ImGui::Selectable("Health", pickUpType == PickUpType::Health)) pickUpType = PickUpType::Health;
        if (ImGui::Selectable("Ammo", pickUpType == PickUpType::Ammo)) pickUpType = PickUpType::Ammo;
        ImGui::EndCombo();
    }
}

const char* getKnockbackMethodLabel(KnockbackMethod knockbackMethod)
{
    switch (knockbackMethod) {
    case KnockbackMethod::Fixed:
        return "Fixed";
    case KnockbackMethod::Auto:
    default:
        return "Auto";
    }
}

void drawKnockbackMethodCombo(const char* label, KnockbackMethod& knockbackMethod)
{
    if (ImGui::BeginCombo(label, getKnockbackMethodLabel(knockbackMethod))) {
        if (ImGui::Selectable("Auto", knockbackMethod == KnockbackMethod::Auto)) knockbackMethod = KnockbackMethod::Auto;
        if (ImGui::Selectable("Fixed", knockbackMethod == KnockbackMethod::Fixed)) knockbackMethod = KnockbackMethod::Fixed;
        ImGui::EndCombo();
    }
}

const char* getSpawnMethodLabel(SpawnMethod spawnMethod)
{
    switch (spawnMethod) {
    case SpawnMethod::Fixed:
        return "Fixed";
    case SpawnMethod::Random:
    default:
        return "Random";
    }
}

void drawSpawnMethodCombo(const char* label, SpawnMethod& spawnMethod)
{
    if (ImGui::BeginCombo(label, getSpawnMethodLabel(spawnMethod))) {
        if (ImGui::Selectable("Random", spawnMethod == SpawnMethod::Random)) spawnMethod = SpawnMethod::Random;
        if (ImGui::Selectable("Fixed", spawnMethod == SpawnMethod::Fixed)) spawnMethod = SpawnMethod::Fixed;
        ImGui::EndCombo();
    }
}

const char* getTextStyleLabel(sf::Text::Style style)
{
    switch (style) {
    case sf::Text::Style::Bold:
        return "Bold";
    case sf::Text::Style::Italic:
        return "Italic";
    case sf::Text::Style::Underlined:
        return "Underlined";
    case sf::Text::Style::StrikeThrough:
        return "StrikeThrough";
    case sf::Text::Style::Regular:
    default:
        return "Regular";
    }
}

void drawTextStyleCombo(const char* label, Text& text)
{
    if (ImGui::BeginCombo(label, getTextStyleLabel(text.style))) {
        if (ImGui::Selectable("Regular", text.style == sf::Text::Style::Regular)) text.style = sf::Text::Style::Regular;
        if (ImGui::Selectable("Bold", text.style == sf::Text::Style::Bold)) text.style = sf::Text::Style::Bold;
        if (ImGui::Selectable("Italic", text.style == sf::Text::Style::Italic)) text.style = sf::Text::Style::Italic;
        if (ImGui::Selectable("Underlined", text.style == sf::Text::Style::Underlined)) text.style = sf::Text::Style::Underlined;
        if (ImGui::Selectable("StrikeThrough", text.style == sf::Text::Style::StrikeThrough)) text.style = sf::Text::Style::StrikeThrough;
        ImGui::EndCombo();
        text.applyProperties();
    }
}

const char* getGameValueLabel(GameValue value)
{
    switch (value) {
    case GameValue::PlayerHealth:
        return "Player Health";
    case GameValue::Coins:
        return "Coins";
    case GameValue::Score:
        return "Score";
    case GameValue::Experience:
        return "Experience";
    default:
        return "Player Health";
    }
}

void drawGameValueCombo(const char* label, Text& text)
{
    if (ImGui::BeginCombo(label, getGameValueLabel(text.linkedGameValue))) {
        if (ImGui::Selectable("Player Health", text.linkedGameValue == GameValue::PlayerHealth)) text.linkedGameValue = GameValue::PlayerHealth;
        if (ImGui::Selectable("Coins", text.linkedGameValue == GameValue::Coins)) text.linkedGameValue = GameValue::Coins;
        if (ImGui::Selectable("Score", text.linkedGameValue == GameValue::Score)) text.linkedGameValue = GameValue::Score;
        if (ImGui::Selectable("Experience", text.linkedGameValue == GameValue::Experience)) text.linkedGameValue = GameValue::Experience;
        ImGui::EndCombo();
    }
}
}


void EditorUI::buildUI(Scene &scene, ResourceManager &resourceManager, SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, bool& isPlaying, sf::View& camera)
{
    if (isSceneLoaded) {
        if (!hideEditorPanels) {
            buildSceneHierarchy(scene);
            if (selectedEntity && selectedEntity->isAlive()) {
                buildEntityInspector(scene, resourceManager);
            }
            if (entityCreatorOpen) {
                buildEntityCreator(scene);
            }
            if (componentPopupOpen) {
                buildAddComponentPopup(scene, resourceManager);
            }
            if (saveSceneAsOpen) {
                buildSaveSceneAsPanel(sceneSerializer,sceneLoader, scene, resourceManager, camera);
            }
            if (createScriptPopupOpen) {
                buildCreateScriptPopup();
            }
            if (openScriptEditor) {
                buildScriptEditor();
            }
            buildScenePanel(scene, resourceManager, sceneSerializer, sceneLoader, isPlaying, camera);
            buildToolBarPanel(sceneSerializer, sceneLoader, scene, resourceManager, isPlaying, camera);
        }
        else
        buildHiddenEditorPanel();
    }
    else {
        buildSceneSelect(scene, sceneLoader, resourceManager, camera);
    }
    if (newSceneOpen) {
        buildNewScenePanel(sceneSerializer, sceneLoader, scene, resourceManager, camera);
    }
}

void EditorUI::buildSceneSelect(Scene& scene, SceneLoader& sceneLoader, ResourceManager& resourceManager, sf::View& camera) {
    std::vector<SceneFile> scenes = scene.loadSceneList(sceneFolderPath);
    ImGui::Begin("Select Scene");
    for (const auto& file : scenes) {
        bool isSelected = file.path == scene.getCurrentScenePath();
        if (ImGui::Selectable(file.name.c_str(), isSelected)) {
            if (sceneLoader.loadFromFile(file.path.string(), scene, resourceManager)) {
                camera.setCenter(VIEWPORT_CENTER_POSITION);
                isSceneLoaded = true;
            }
        }
    }
    if (ImGui::Button("Create New Scene")) {
        newSceneOpen = true;
    }
    ImGui::End();
    
}

void EditorUI::handleDragMove(sf::RenderWindow& window, bool isPlaying)
{
    bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (isPlaying || !dragMoveEnabled || !mouseDown || ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    if (selectedEntity && (selectedEntity->getComponentMask() & ComponentTypes::Transform) != 0) {
        auto transform = selectedEntity->getComponent<Transform>();
        sf::Vector2i pixelPosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPosition = window.mapPixelToCoords(pixelPosition);
        transform->setWorldPosition(worldPosition);
    }
}

void EditorUI::buildSceneHierarchy(Scene& scene)
{
    ImGui::Begin("Scene Hierarchy");
    for (const auto& entity : scene.getEntities()) {
        bool isSelected = selectedEntity == entity;
        if (ImGui::Selectable(entity->getName().c_str(), isSelected)) {
            selectedEntity = entity;
        }
    }
    ImGui::Separator();
    if (ImGui::Button("Create Entity")) {
        entityCreatorOpen = true;
    }
    ImGui::End();
}

void EditorUI::buildEntityCreator(Scene& scene) {
    ImGui::Begin("Create Entity");
    static char entityName[128] = "";
    ImGui::InputText("Name", entityName, IM_ARRAYSIZE(entityName));
    if (ImGui::Button("Create")) {
        if (strlen(entityName) > 0) {
            std::shared_ptr<Entity> newEntity = scene.createEntity(entityName);
            selectedEntity = newEntity;
            entityName[0] = '\0'; // Clear the input field
            entityCreatorOpen = false; // Close the entity creator window
        }
    }
    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
        entityCreatorOpen = false; // Close the entity creator window
    }
    ImGui::End();
}

void EditorUI::duplicateSelectedEntity(Scene& scene, ResourceManager& resourceManager)
{
    if (!selectedEntity || !selectedEntity->isAlive()) {
        return;
    }

    auto duplicatedEntity = scene.createEntity(makeDuplicateEntityName(scene, selectedEntity->getName()));
    const auto componentMask = selectedEntity->getComponentMask();

    if ((componentMask & ComponentTypes::Transform) != 0) {
        auto transform = selectedEntity->getComponent<Transform>();
        auto duplicatedTransform = std::make_shared<Transform>(*transform);
        duplicatedTransform->moveWorldPosition(sf::Vector2f{32.f, 32.f});
        duplicatedEntity->addComponent(duplicatedTransform, ComponentTypes::Transform);
    }
    if ((componentMask & ComponentTypes::Sprite) != 0) {
        auto sprite = selectedEntity->getComponent<Sprite>();
        std::string textureID = resourceManager.hasTexture(sprite->texID) ? sprite->texID : ResourceManager::DefaultTextureID;
        auto duplicatedSprite = std::make_shared<Sprite>(resourceManager.getTexture(textureID), textureID);
        if (sprite->sprite) {
            duplicatedSprite->sprite->setTextureRect(sprite->sprite->getTextureRect());
        }
        duplicatedEntity->addComponent(duplicatedSprite, ComponentTypes::Sprite);
    }
    if ((componentMask & ComponentTypes::Physics) != 0) {
        auto physics = selectedEntity->getComponent<Physics>();
        duplicatedEntity->addComponent(std::make_shared<Physics>(*physics), ComponentTypes::Physics);
    }
    if ((componentMask & ComponentTypes::Collision) != 0) {
        auto collision = selectedEntity->getComponent<Collision>();
        duplicatedEntity->addComponent(std::make_shared<Collision>(*collision), ComponentTypes::Collision);
    }
    if ((componentMask & ComponentTypes::Input) != 0) {
        auto input = selectedEntity->getComponent<Input>();
        duplicatedEntity->addComponent(std::make_shared<Input>(*input), ComponentTypes::Input);
    }
    if ((componentMask & ComponentTypes::Health) != 0) {
        auto health = selectedEntity->getComponent<Health>();
        duplicatedEntity->addComponent(std::make_shared<Health>(*health), ComponentTypes::Health);
    }
    if ((componentMask & ComponentTypes::PickUp) != 0) {
        auto pickUp = selectedEntity->getComponent<PickUp>();
        duplicatedEntity->addComponent(std::make_shared<PickUp>(*pickUp), ComponentTypes::PickUp);
    }
    if ((componentMask & ComponentTypes::ContactDamage) != 0) {
        auto contactDamage = selectedEntity->getComponent<ContactDamage>();
        duplicatedEntity->addComponent(std::make_shared<ContactDamage>(*contactDamage), ComponentTypes::ContactDamage);
    }
    if ((componentMask & ComponentTypes::AI) != 0) {
        auto ai = selectedEntity->getComponent<AI>();
        duplicatedEntity->addComponent(std::make_shared<AI>(*ai), ComponentTypes::AI);
    }
    if ((componentMask & ComponentTypes::Shooter) != 0) {
        auto shooter = selectedEntity->getComponent<Shooter>();
        duplicatedEntity->addComponent(std::make_shared<Shooter>(*shooter), ComponentTypes::Shooter);
    }
    if ((componentMask & ComponentTypes::LifeTime) != 0) {
        auto lifeTime = selectedEntity->getComponent<LifeTime>();
        auto duplicatedLifeTime = std::make_shared<LifeTime>(*lifeTime);
        duplicatedEntity->addComponent(duplicatedLifeTime, ComponentTypes::LifeTime);
    }
    if ((componentMask & ComponentTypes::Spawner) != 0) {
        auto spawner = selectedEntity->getComponent<Spawner>();
        duplicatedEntity->addComponent(std::make_shared<Spawner>(*spawner), ComponentTypes::Spawner);
    }
    if ((componentMask & ComponentTypes::Door) != 0) {
        auto door = selectedEntity->getComponent<Door>();
        duplicatedEntity->addComponent(std::make_shared<Door>(*door), ComponentTypes::Door);
    }
    if ((componentMask & ComponentTypes::Key) != 0) {
        auto key = selectedEntity->getComponent<Key>();
        duplicatedEntity->addComponent(std::make_shared<Key>(*key), ComponentTypes::Key);
    }
    if ((componentMask & ComponentTypes::Script) != 0) {
        auto script = selectedEntity->getComponent<Script>();
        auto duplicatedScript = std::make_shared<Script>(*script);
        duplicatedScript->started = false;
        duplicatedEntity->addComponent(duplicatedScript, ComponentTypes::Script);
    }
    if ((componentMask & ComponentTypes::Text) != 0) {
        auto text = selectedEntity->getComponent<Text>();
        std::string fontID = resourceManager.hasFont(text->fontID) ? text->fontID : ResourceManager::DefaultFontID;
        auto duplicatedText = std::make_shared<Text>(resourceManager.getFont(fontID), fontID);
        duplicatedText->style = text->style;
        duplicatedText->linkGameValue = text->linkGameValue;
        duplicatedText->positionRelativeToViewport = text->positionRelativeToViewport;
        duplicatedText->linkedGameValue = text->linkedGameValue;
        duplicatedText->characterSize = text->characterSize;
        duplicatedText->textString = text->textString;
        duplicatedText->fillColor = text->fillColor;
        duplicatedText->applyProperties();
        duplicatedEntity->addComponent(duplicatedText, ComponentTypes::Text);
    }
    if ((componentMask & ComponentTypes::Switch) != 0) {
        auto switchComponent = selectedEntity->getComponent<Switch>();
        duplicatedEntity->addComponent(std::make_shared<Switch>(*switchComponent), ComponentTypes::Switch);
    }
    if ((componentMask & ComponentTypes::PopText) != 0) {
        auto popText = selectedEntity->getComponent<PopText>();
        std::string fontID = resourceManager.hasFont(popText->fontID) ? popText->fontID : ResourceManager::DefaultFontID;
        auto duplicatedPopText = std::make_shared<PopText>(resourceManager.getFont(fontID), fontID);
        duplicatedPopText->offset = popText->offset;
        duplicatedPopText->textString = popText->textString;
        duplicatedPopText->size = popText->size;
        duplicatedPopText->color = popText->color;
        duplicatedPopText->style = popText->style;
        duplicatedPopText->lifeTimeDuration = popText->lifeTimeDuration;
        duplicatedPopText->applyProperties();
        duplicatedEntity->addComponent(duplicatedPopText, ComponentTypes::PopText);
    }
    if ((componentMask & ComponentTypes::Loot) != 0) {
        auto loot = selectedEntity->getComponent<Loot>();
        duplicatedEntity->addComponent(std::make_shared<Loot>(*loot), ComponentTypes::Loot);
    }

    selectedEntity = duplicatedEntity;
}

void EditorUI::buildRenameEntityPopup()
{
    ImGui::Begin("Rename Entity");
    if (!selectedEntity || !selectedEntity->isAlive()) {
        renameEntityPopupOpen = false;
        ImGui::End();
        return;
    }
    static char renameEntityName[128] = "";
    ImGui::InputText("Name", renameEntityName, IM_ARRAYSIZE(renameEntityName));
    if (ImGui::Button("Rename")) {
        if (std::strlen(renameEntityName) > 0) {
            selectedEntity->setName(renameEntityName);
            renameEntityPopupOpen = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        renameEntityPopupOpen = false;
    }
    ImGui::End();
}

void EditorUI::buildAddComponentPopup(Scene &scene, ResourceManager& resourceManager)
{
    ImGui::Begin("Add Component");
    for (const auto& componentName : componentNames) {
        if (ImGui::Selectable(componentName.c_str())) {
                if (componentName == "Transform") {
                    auto transform = std::make_shared<Transform>();
                    selectedEntity->addComponent(transform, ComponentTypes::Transform);
                }
                else if (componentName == "Sprite") {
                    auto sprite = std::make_shared<Sprite>(resourceManager.getTexture(ResourceManager::DefaultTextureID), "default");
                    selectedEntity->addComponent(sprite, ComponentTypes::Sprite);
                }
                else if (componentName == "Physics") {
                    auto physics = std::make_shared<Physics>();
                    selectedEntity->addComponent(physics, ComponentTypes::Physics);
                }
                else if (componentName == "Collision") {
                    auto collision = std::make_shared<Collision>();
                    selectedEntity->addComponent(collision, ComponentTypes::Collision);
                }
                else if (componentName == "Input") {
                    auto input = std::make_shared<Input>();
                    selectedEntity->addComponent(input, ComponentTypes::Input);
                }
                else if (componentName == "Health") {
                    auto health = std::make_shared<Health>();
                    
                    selectedEntity->addComponent(health, ComponentTypes::Health);
                }
                else if (componentName == "PickUp") {
                    auto pickUp = std::make_shared<PickUp>();
                    selectedEntity->addComponent(pickUp, ComponentTypes::PickUp);
                }
                else if (componentName == "ContactDamage") {
                    auto contactDamage = std::make_shared<ContactDamage>();
                    selectedEntity->addComponent(contactDamage, ComponentTypes::ContactDamage);
                }
                else if (componentName == "AI") {
                    auto ai = std::make_shared<AI>();
                    ai->defaultMovementState = PlatformerMovementStates::Strafe;
                    ai->currentMovementState = ai->defaultMovementState;
                    selectedEntity->addComponent(ai, ComponentTypes::AI);
                }
                else if (componentName == "Shooter") {
                    auto shooter = std::make_shared<Shooter>();
                    selectedEntity->addComponent(shooter, ComponentTypes::Shooter);
                }
                else if (componentName == "LifeTime") {
                    auto lifeTime = std::make_shared<LifeTime>(0.0f);
                    selectedEntity->addComponent(lifeTime, ComponentTypes::LifeTime);
                }
                else if (componentName == "Spawner") {
                    auto spawner = std::make_shared<Spawner>();
                    selectedEntity->addComponent(spawner, ComponentTypes::Spawner);
                }
                else if (componentName == "Door") {
                    auto door = std::make_shared<Door>();
                    door->unlockedTexID = resourceManager.DefaultTextureID;
                    selectedEntity->addComponent(door, ComponentTypes::Door);
                }
                else if (componentName == "Key") {
                    auto key = std::make_shared<Key>();
                    selectedEntity->addComponent(key, ComponentTypes::Key);
                }
                else if (componentName == "Script") {
                    auto script = std::make_shared<Script>();
                    selectedEntity->addComponent(script, ComponentTypes::Script);
                    createScriptPopupOpen = true;
                }
                else if (componentName == "Text") {
                    auto text = std::make_shared<Text>(resourceManager.getFont(ResourceManager::DefaultFontID), ResourceManager::DefaultFontID);
                    selectedEntity->addComponent(text, ComponentTypes::Text);
                }
                else if (componentName == "Switch") {
                    auto switchComponent = std::make_shared<Switch>();
                    switchComponent->activatedTextureID = "switch_sprite_on";
                    switchComponent->deactivatedTextureID = "switch_sprite_off";
                    selectedEntity->addComponent(switchComponent, ComponentTypes::Switch);
                }
                else if (componentName == "PowerUp") {
                    auto powerUp = std::make_shared<PowerUp>();
                    selectedEntity->addComponent(powerUp, ComponentTypes::PowerUp);
                }
                else if (componentName == "PopText") {
                    auto popText = std::make_shared<PopText>(resourceManager.getFont(ResourceManager::DefaultFontID), ResourceManager::DefaultFontID);
                    selectedEntity->addComponent(popText, ComponentTypes::PopText);
                }
                else if (componentName == "Loot") {
                    auto loot = std::make_shared<Loot>();
                    selectedEntity->addComponent(loot, ComponentTypes::Loot);
                }
                
            componentPopupOpen = false; // Close the popup after adding a component
            
        }
    }
    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
        componentPopupOpen = false;
    }
    
    ImGui::End();

}

void EditorUI::buildCreateScriptPopup() {
    ImGui::Begin("Script Configuration");
    auto entityScript = selectedEntity->getComponent<Script>();
    if (!entityScript) {
        createScriptPopupOpen = false;
        ImGui::End();
        return;
    }
    static char newScriptName[128] = "";
    ImGui::InputText("Name", newScriptName, IM_ARRAYSIZE(newScriptName));
    if (ImGui::Button("Create and Link New Script")) {
        std::string scriptName = newScriptName;
        if (!scriptName.empty()) {
            if (scriptName.length() >= 5 && scriptName.substr(scriptName.length() - 5) == ".json"){
                scriptName.erase(scriptName.length() - 5);
            }
            scriptName += ".json";
            std::filesystem::path newScriptFilePath = SCRIPTS_FILE_PATH / scriptName;
            std::ofstream file(newScriptFilePath);
            if (!file.is_open()) {
                std::cerr << "Failed to create file: " << newScriptFilePath << std::endl;
            }
            else {
                entityScript->filePath = newScriptFilePath.string();
                entityScript->fileName = newScriptFilePath.stem().string();
                newScriptName[0] = '\0';
                createScriptPopupOpen = false;
                openScriptInEditor(entityScript->fileName, entityScript->filePath);
            }
        }
    }
    
    ImGui::Separator();
    
    std::vector<std::filesystem::path> scripts = Script::loadScriptsList(SCRIPTS_FILE_PATH);
    const char* scriptPathPreview = "none";
    for (const auto scriptPath : scripts) {
        if (scriptPath.string() == entityScript->filePath) scriptPathPreview = entityScript->fileName.c_str();
    }
    if (ImGui::BeginCombo("Select Existing Script: ", scriptPathPreview)) {
        for (const auto& scriptPath : scripts) {
            bool isSelected = scriptPath.string() == entityScript->filePath;
            if (ImGui::Selectable(scriptPath.string().c_str(), isSelected)) {
                entityScript->filePath = scriptPath.string();
                entityScript->fileName = scriptPath.stem().string();
                
            }
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Link Existing Script")){
        newScriptName[0] = '\0';
        createScriptPopupOpen = false;
    }
    ImGui::End();
    


}

void EditorUI::openScriptInEditor(std::string fileName, std::string filePath)
{
    currentFileName = fileName;
    currentFilePath = filePath;
    openScriptEditor = true;
}

void EditorUI::buildHiddenEditorPanel()
{
    ImGui::Begin("Hidden");
    if (ImGui::Button("Show Editor")) {
        hideEditorPanels = false;
    }
    ImGui::End();
}

void EditorUI::buildRemoveComponentPopup()
{
    ImGui::Begin("Remove Component");
    if (!selectedEntity || !selectedEntity->isAlive()) {
        removeComponentPopupOpen = false;
        ImGui::End();
        return;
    }

    const auto componentMask = selectedEntity->getComponentMask();
    if ((componentMask & ComponentTypes::Transform) != 0 && ImGui::Selectable("Transform")) {
        selectedEntity->removeComponent<Transform>(ComponentTypes::Transform);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Sprite) != 0 && ImGui::Selectable("Sprite")) {
        selectedEntity->removeComponent<Sprite>(ComponentTypes::Sprite);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Physics) != 0 && ImGui::Selectable("Physics")) {
        selectedEntity->removeComponent<Physics>(ComponentTypes::Physics);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Collision) != 0 && ImGui::Selectable("Collision")) {
        selectedEntity->removeComponent<Collision>(ComponentTypes::Collision);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Input) != 0 && ImGui::Selectable("Input")) {
        selectedEntity->removeComponent<Input>(ComponentTypes::Input);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Health) != 0 && ImGui::Selectable("Health")) {
        selectedEntity->removeComponent<Health>(ComponentTypes::Health);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::PickUp) != 0 && ImGui::Selectable("PickUp")) {
        selectedEntity->removeComponent<PickUp>(ComponentTypes::PickUp);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::ContactDamage) != 0 && ImGui::Selectable("ContactDamage")) {
        selectedEntity->removeComponent<ContactDamage>(ComponentTypes::ContactDamage);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::AI) != 0 && ImGui::Selectable("AI")) {
        selectedEntity->removeComponent<AI>(ComponentTypes::AI);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Shooter) != 0 && ImGui::Selectable("Shooter")) {
        selectedEntity->removeComponent<Shooter>(ComponentTypes::Shooter);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::LifeTime) != 0 && ImGui::Selectable("LifeTime")) {
        selectedEntity->removeComponent<LifeTime>(ComponentTypes::LifeTime);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Spawner) != 0 && ImGui::Selectable("Spawner")) {
        selectedEntity->removeComponent<Spawner>(ComponentTypes::Spawner);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Door) != 0 && ImGui::Selectable("Door")) {
        selectedEntity->removeComponent<Door>(ComponentTypes::Door);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Key) != 0 && ImGui::Selectable("Key")) {
        selectedEntity->removeComponent<Key>(ComponentTypes::Key);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Script) != 0 && ImGui::Selectable("Script")) {
        selectedEntity->removeComponent<Script>(ComponentTypes::Script);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Text) != 0 && ImGui::Selectable("Text")) {
        selectedEntity->removeComponent<Text>(ComponentTypes::Text);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Switch) != 0 && ImGui::Selectable("Switch")) {
        selectedEntity->removeComponent<Switch>(ComponentTypes::Switch);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::PowerUp) != 0 && ImGui::Selectable("PowerUp")) {
        selectedEntity->removeComponent<PowerUp>(ComponentTypes::PowerUp);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::PopText) != 0 && ImGui::Selectable("PopText")) {
        selectedEntity->removeComponent<PopText>(ComponentTypes::PopText);
        removeComponentPopupOpen = false;
    }
    if ((componentMask & ComponentTypes::Loot) != 0 && ImGui::Selectable("Loot")) {
        selectedEntity->removeComponent<Loot>(ComponentTypes::Loot);
        removeComponentPopupOpen = false;
    }

    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
        removeComponentPopupOpen = false;
    }
    ImGui::End();
}

void EditorUI::buildScenePanel(Scene& scene, ResourceManager& resourceManager, SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, bool& isPlaying, sf::View& camera)
{
    ImGui::Begin("Scene Configuration");

    const char* preview = "none";
    auto playerEntity = scene.getPlayerEntity();
    for (const auto& option : scene.getEntities()) {
        if (option == playerEntity) preview = option->getName().c_str();
    }
    if (ImGui::BeginCombo("Player Entity", preview)) {
        for (const auto& option : scene.getEntities()) {
            bool isSelected = option == playerEntity;
            if (ImGui::Selectable(option->getName().c_str(), isSelected)) scene.setPlayerEntity(option);
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
    ImGui::Text("Player Stats");
    ImGui::Text("Coins: %d", scene.getGameState().coins);
    ImGui::Text("Score: %d", scene.getGameState().score);
    ImGui::Text("Experience: %.2f", scene.getGameState().experience);

    if (ImGui::SliderFloat("Scene Gravity", &GRAVITY, 0.0f, 2000.0f)) {
        for (const auto& entity : scene.getEntities()) {
            if ((entity->getComponentMask() & ComponentTypes::Physics) != 0) {
                auto physics = entity->getComponent<Physics>();
                if (physics && physics->useGravity) {
                    physics->gravity = GRAVITY;
                }
            }
        }
    }
    ImGui::SliderFloat("Viewport Camera Speed", &VIEWPORT_CAMERA_SPEED, 5.f, 1000.f);
    ImGui::Checkbox("Camera Follows Player Character?", &VIEWPORT_FOLLOW_PLAYER_CHARACTER);
    ImGui::SliderFloat2("Viewport Camera Offset", &VIEWPORT_CAMERA_OFFSET.x, -1000.f, 1000.f);
    if (ImGui::Button("Zero Camera Position")) {
        camera.setCenter({WINDOW_SIZE.x / 2, WINDOW_SIZE.y / 2});
        VIEWPORT_CENTER_POSITION = camera.getCenter();
    }
    
    ImGui::Separator();

    float backgroundColor[4] = {
        scene.backgroundColor.r / 255.0f,
        scene.backgroundColor.g / 255.0f,
        scene.backgroundColor.b / 255.0f,
        scene.backgroundColor.a / 255.0f
    };
    if (ImGui::ColorEdit4("Background Color", backgroundColor)) {
        scene.backgroundColor = sf::Color(
            static_cast<std::uint8_t>(backgroundColor[0] * 255.0f),
            static_cast<std::uint8_t>(backgroundColor[1] * 255.0f),
            static_cast<std::uint8_t>(backgroundColor[2] * 255.0f),
            static_cast<std::uint8_t>(backgroundColor[3] * 255.0f)
        );
    }
    ImGui::End();
}


void EditorUI::buildSaveSceneAsPanel(SceneSerializer& sceneSerializer,SceneLoader& sceneLoader, Scene& scene, ResourceManager& resourceManager, sf::View& camera) {
    ImGui::Begin("Save Scene As");
    static char sceneName[128] = "";
    ImGui::InputText("Save Scene As", sceneName, IM_ARRAYSIZE(sceneName));
    if (ImGui::Button("Save As")) {
        std::string newSceneFilePath = sceneFolderPath.string() + "/" + sceneName + ".json";
        std::cout << newSceneFilePath << std::endl;
        sceneSerializer.saveToFile(newSceneFilePath, scene, resourceManager);
        scene.clear();
        if (sceneLoader.loadFromFile(newSceneFilePath, scene, resourceManager)) {
            camera.setCenter(VIEWPORT_CENTER_POSITION);
        }
        saveSceneAsOpen = false;
    }
    if (ImGui::Button("Cancel")) {
        saveSceneAsOpen = false;
    }

    ImGui::End();

}

void EditorUI::buildNewScenePanel(SceneSerializer &sceneSerializer, SceneLoader &sceneLoader, Scene &scene, ResourceManager &resourceManager, sf::View& camera)
{
    ImGui::Begin("Create New Scene");
    static char sceneName[128] = "";
    ImGui::InputText("New Scene Name", sceneName, IM_ARRAYSIZE(sceneName));
    if (ImGui::Button("Create New Scene")) {
        std::string newSceneFilePath = sceneFolderPath.string() + "/" + sceneName + ".json";
        std::cout << newSceneFilePath << std::endl;
        scene.clear();
        scene.setCurrentScenePath(newSceneFilePath);
        sceneSerializer.saveToFile(newSceneFilePath, scene, resourceManager);
        if (sceneLoader.loadFromFile(newSceneFilePath, scene, resourceManager)) {
            camera.setCenter(VIEWPORT_CENTER_POSITION);
        }
        newSceneOpen = false;
    }
    if (ImGui::Button("Cancel")) {
        newSceneOpen = false;
    }
    ImGui::End(); 
}

void EditorUI::buildToolBarPanel(SceneSerializer &sceneSerializer, SceneLoader &sceneLoader, Scene &scene, ResourceManager &resourceManager, bool& isPlaying, sf::View& camera)
{
    ImGui::Begin("ToolBar");
        if (isPlaying) {
        if (ImGui::Button("Stop")) {
            isPlaying = false;
            if (sceneLoader.loadFromFile(scene.getCurrentScenePath().string(), scene, resourceManager)) {
                camera.setCenter(VIEWPORT_CENTER_POSITION);
            }
            selectedEntity = nullptr;
        }
    }
    else {
        if (ImGui::Button("Play")) {
            if (sceneSerializer.saveToFile(scene.getCurrentScenePath().string(), scene, resourceManager)) {
                isPlaying = true;
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Scene")) {
        sceneSerializer.saveToFile(scene.getCurrentScenePath().string(), scene, resourceManager);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load New Scene")) {
        scene.clear();
        scene.clearCurrentScenePath();
        isSceneLoaded = false;
    }
    // implement these two features then nail down final features to complete engine 
    ImGui::SameLine();
    if (ImGui::Button("New Scene")) {
        newSceneOpen = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload Current Scene")) {
        if (sceneLoader.loadFromFile(scene.getCurrentScenePath().string(), scene, resourceManager)) {
            camera.setCenter(VIEWPORT_CENTER_POSITION);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Scene As")) {
        saveSceneAsOpen = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Hide Editor")) {
        hideEditorPanels = true;
    }
    ImGui::End();
}

void EditorUI::buildScriptEditor()
{

    static bool initialized = false;

    if (!initialized) {
        editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        initialized = true;
    }

    ImGui::Begin(currentFileName.c_str());
    if (ImGui::Button("Save & Apply")) {
        try {
            const std::string scriptText = editor.GetText();
            auto data = nlohmann::json::parse(scriptText);
            if (selectedEntity) {
                auto script = selectedEntity->getComponent<Script>();
                if (script) {
                    loadScriptFromJson(data, *script);
                    script->filePath = currentFilePath;
                    script->fileName = currentFileName;
                }
            }
            writeStringIntoJsonFile(scriptText, currentFilePath);
            openScriptEditor = false;
        }
        catch (const std::exception& error) {
            std::cerr << "Failed to apply script JSON: " << error.what() << std::endl;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        openScriptEditor = false;
    }
    ImGui::Separator();
    editor.Render("ScriptTextEditor");

    ImGui::End();
}

void EditorUI::buildEntityInspector(Scene& scene, ResourceManager& resourceManager)
{
    // redundancy
    ImGui::Begin("Entity Inspector");
    if (!selectedEntity || !selectedEntity->isAlive()) {
        selectedEntity = nullptr;
        ImGui::End();
        return;
    }

    ImGui::Text("Selected: %s", selectedEntity->getName().c_str());
    bool isBlueprint = selectedEntity->isBlueprint();
    if (ImGui::Checkbox("Blueprint Only", &isBlueprint)) {
        selectedEntity->setBlueprint(isBlueprint);
    }

    auto transform = selectedEntity->getComponent<Transform>();
    if (transform) {
        ImGui::Separator();
        ImGui::Text("Transform");
        sf::Vector2f position = transform->getWorldPosition();
        sf::Vector2f localPosition = transform->getLocalPosition();
        sf::Vector2f scale = transform->getScale();
        float rotation = transform->getRotation();

        if (ImGui::DragFloat2("Position", &position.x, 1.0f)) {
            transform->setWorldPosition(position);
        }
        if (ImGui::DragFloat2("Local Position", &localPosition.x, 1.0f)) {
            transform->setLocalPosition(localPosition);
        }
        if (ImGui::DragFloat2("Scale", &scale.x, 0.5f)) {
            transform->setScale(scale);
        }
        if (ImGui::DragFloat("Rotation", &rotation, 1.0f)) {
            transform->setRotation(rotation);
        }
        ImGui::Checkbox("Drag Move", &dragMoveEnabled);
    }

    auto physics = selectedEntity->getComponent<Physics>();
    if (physics) {
        ImGui::Separator();
        ImGui::Text("Physics");
        ImGui::Text("Velocity: (%.2f, %.2f)", physics->velocity.x, physics->velocity.y);
        ImGui::Text("Acceleration: (%.2f, %.2f)", physics->acceleration.x, physics->acceleration.y);
        ImGui::Checkbox("Use Gravity", &physics->useGravity);
        ImGui::Checkbox("Use Acceleration", &physics->useAcceleration);
        if (ImGui::Button("Zero-Out Velocity")) {
            physics->velocity *= 0.f;
        }
    }

    auto collision = selectedEntity->getComponent<Collision>();
    if (collision) {
        ImGui::Separator();
        ImGui::Text("Collision");
        ImGui::DragFloat2("Size", &collision->size.x, 1.0f);
        ImGui::DragFloat2("Offset", &collision->offset.x, 1.0f);
        ImGui::Checkbox("Static", &collision->isStatic);
        ImGui::Checkbox("Physical Collider", &collision->isPhysicalCollider);
        ImGui::Checkbox("Resolve Physical Collisions With Team", &collision->resolvePhysicalCollisionsWithTeam);
        drawFactionCombo("Collision Faction", collision->faction);
    }

    auto sprite = selectedEntity->getComponent<Sprite>();
    if (sprite) {
        ImGui::Separator();
        ImGui::Text("Sprite");
        const auto& preview = sprite->texID.c_str();

        if (ImGui::BeginCombo("Texture ID", preview)) {
            for (const auto& option : resourceManager.getTextures()) {
                bool isSelected = option.first == sprite->texID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) {
                    sprite->texID = option.first;
                    sprite->sprite->setTexture(resourceManager.getTexture(sprite->texID));
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    auto input = selectedEntity->getComponent<Input>();  
    if (input) {
        ImGui::Separator();
        ImGui::Text("Input");
        const char* inputMovementTypePreview = input->movementType == MovementType::TopDown ? "Top Down" : "Platformer";
        if (ImGui::BeginCombo("Input Movement Type", inputMovementTypePreview)) {
            if (ImGui::Selectable("Platformer", input->movementType == MovementType::Platformer)) input->movementType = MovementType::Platformer;
            if (ImGui::Selectable("Top Down", input->movementType == MovementType::TopDown)) input->movementType = MovementType::TopDown;
            ImGui::EndCombo();
        }
        ImGui::DragFloat("Move Speed", &input->moveSpeed, 1.0f);
        ImGui::DragFloat("Jump Force", &input->jumpForce, 1.0f);
        ImGui::DragFloat("Acceleration", &input->acceleration, 1.0f, 1.0f, input->moveSpeed);
        ImGui::DragFloat("Deceleration", &input->deceleration, 1.0f, 1.0f, input->moveSpeed);
    }

    auto health = selectedEntity->getComponent<Health>();
    if (health) {
        ImGui::Separator();
        ImGui::Text("Health");
        ImGui::DragFloat("Current Health", &health->currentHealth, 1.0f);
        ImGui::DragFloat("Max Health", &health->maxHealth, 1.0f);
    }
    auto contactDamage = selectedEntity->getComponent<ContactDamage>();
    if (contactDamage) {
        ImGui::Separator();
        ImGui::Text("Contact Damage");
        ImGui::DragFloat("damage", &contactDamage->damage, 1.0f);
        ImGui::DragFloat("knockback", &contactDamage->knockback, 1.0f, 1.0f);
        drawKnockbackMethodCombo("Knockback Method", contactDamage->knockbackMethod);
        if (contactDamage->knockbackMethod == KnockbackMethod::Fixed) {
            ImGui::DragFloat2("Knockback Direction", &contactDamage->knockbackDirection.x, 1.0f);
        }
        ImGui::Checkbox("Destroy Entity On Contact", &contactDamage->destroyEntityOnContact);
    }

    auto ai = selectedEntity->getComponent<AI>();
    if (ai) {
        ImGui::Separator();
        ImGui::Text("AI");
        const char* movementTypePreview = ai->movementType == MovementType::TopDown ? "Top Down" : "Platformer";
        if (ImGui::BeginCombo("Movement Type", movementTypePreview)) {
            if (ImGui::Selectable("Platformer", ai->movementType == MovementType::Platformer)) ai->movementType = MovementType::Platformer;
            if (ImGui::Selectable("Top Down", ai->movementType == MovementType::TopDown)) ai->movementType = MovementType::TopDown;
            ImGui::EndCombo();
        }
        ImGui::DragFloat("AI Move Speed", &ai->moveSpeed, 1.0f);
        ImGui::DragFloat("AI Acceleration", &ai->acceleration, 1.0f, 1.0f, ai->moveSpeed);
        ImGui::DragFloat("AI Deceleration", &ai->deceleration, 1.0f, 1.0f, ai->moveSpeed);
        if (ai->movementType == MovementType::Platformer) {
        ImGui::DragFloat("Aggro Range", &ai->aggroRange, 1.0f);
        ImGui::DragFloat("Strafe Distance", &ai->strafeDistance, 1.0f, 0.0f);
        ImGui::Checkbox("Vertical Strafe", &ai->verticalStrafe);

        
        const char* movementPreview = "Strafe";
        if (ai->currentMovementState == PlatformerMovementStates::Follow) movementPreview = "Follow";
        if (ai->currentMovementState == PlatformerMovementStates::Idle) movementPreview = "Idle";
        if (ImGui::BeginCombo("Current Movement", movementPreview)) {
            if (ImGui::Selectable("Strafe", ai->currentMovementState == PlatformerMovementStates::Strafe)) ai->currentMovementState = PlatformerMovementStates::Strafe;
            if (ImGui::Selectable("Follow", ai->currentMovementState == PlatformerMovementStates::Follow)) ai->currentMovementState = PlatformerMovementStates::Follow;
            if (ImGui::Selectable("Idle", ai->currentMovementState == PlatformerMovementStates::Idle)) ai->currentMovementState = PlatformerMovementStates::Idle;
            ImGui::EndCombo();
        }
        const char* defaultPreview = "Strafe";
        if (ai->defaultMovementState == PlatformerMovementStates::Follow) defaultPreview = "Follow";
        if (ai->defaultMovementState == PlatformerMovementStates::Idle) defaultPreview = "Idle";
        if (ImGui::BeginCombo("Default Movement", defaultPreview)) {
            if (ImGui::Selectable("Strafe", ai->defaultMovementState == PlatformerMovementStates::Strafe)) ai->defaultMovementState = PlatformerMovementStates::Strafe;
            if (ImGui::Selectable("Follow", ai->defaultMovementState == PlatformerMovementStates::Follow)) ai->defaultMovementState = PlatformerMovementStates::Follow;
            if (ImGui::Selectable("Idle", ai->defaultMovementState == PlatformerMovementStates::Idle)) ai->defaultMovementState = PlatformerMovementStates::Idle;
            ImGui::EndCombo();
        }
        ImGui::Checkbox("Can Follow", &ai->canFollow);
        }
    }

    auto shooter = selectedEntity->getComponent<Shooter>();
    if (shooter) {
        ImGui::Separator();
        ImGui::Text("Shooter");
        const auto& shooterTexturePreview = shooter->textureID.c_str();
        if (ImGui::BeginCombo("Projectile Texture ID", shooterTexturePreview)) {
            for (const auto& option : resourceManager.getTextures()) {
                bool isSelected = option.first == shooter->textureID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) shooter->textureID = option.first;
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        const char* gunModeNames[] = {"Default", "Shotgun"};
        int gunMode = static_cast<int>(shooter->gunMode);
        if (ImGui::Combo("Gun Mode", &gunMode, gunModeNames, IM_ARRAYSIZE(gunModeNames))) {
            shooter->gunMode = static_cast<GunMode>(gunMode);
        }
        if (shooter->gunMode == GunMode::Shotgun) {
            ImGui::DragFloat("Shotgun Pellet Count", &shooter->shotGunPelletCount, 1.0f, 1.0f);
        }
        ImGui::DragFloat("Projectile Speed", &shooter->projectileSpeed, 1.0f, 0.0f);
        ImGui::DragFloat("Projectile Damage", &shooter->projectileDamage, 1.0f, 0.0f);
        ImGui::DragFloat2("Projectile Offset", &shooter->offset.x, 1.0f);
        if (ImGui::DragFloat2("Projectile Scale", &shooter->projectileScale.x, 0.01f, 0.01f, 10.0f)) {
            shooter->collisionScaling = {32.f * shooter->projectileScale.x, 32.f * shooter->projectileScale.y};
        }
    }

    auto lifeTime = selectedEntity->getComponent<LifeTime>();
    if (lifeTime) {
        ImGui::Separator();
        ImGui::Text("LifeTime");
        ImGui::DragFloat("Life Time Duration", &lifeTime->lifeTimeDuration, 0.1f, 0.0f);
        ImGui::DragFloat("Life Time Timer", &lifeTime->lifeTimeTimer, 0.1f, 0.0f);
    }

    auto spawner = selectedEntity->getComponent<Spawner>();
    if (spawner) {
        ImGui::Separator();
        ImGui::Text("Spawner");
        char baseNameBuffer[128];
        std::snprintf(baseNameBuffer, sizeof(baseNameBuffer), "%s", spawner->baseName.c_str());
        if (ImGui::InputText("Spawned Name Prefix", baseNameBuffer, sizeof(baseNameBuffer))) {
            spawner->baseName = baseNameBuffer;
        }
        drawSpawnMethodCombo("Spawn Method", spawner->spawnMethod);
        ImGui::DragFloat("Spawn Frequency", &spawner->spawnFrequency, 0.1f, 0.0f);
        if (spawner->spawnMethod == SpawnMethod::Fixed) {
            ImGui::DragFloat2("Spawn Position", &spawner->spawnPosition.x, 1.0f);
        }
    }

    auto pickUp = selectedEntity->getComponent<PickUp>();
    if (pickUp) {
        ImGui::Separator();
        ImGui::Text("PickUp");
        drawPickUpTypeCombo("PickUpType", pickUp->pickUpType);
        ImGui::DragFloat("amount", &pickUp->amount, 1.0f);
    }

    auto door = selectedEntity->getComponent<Door>();
    if (door) {
        ImGui::Separator();
        ImGui::Text("Door");
        const char* scenePreview = "default";
        std::vector<SceneFile> scenes = scene.loadSceneList(sceneFolderPath);
        for (const auto& path : scenes) {
            if (door->sceneFilePath == path.path.string()) scenePreview = path.name.c_str(); 
        }
        if (ImGui::BeginCombo("Scene File Path", scenePreview)) {
            for (const auto& path : scenes) {
                bool isSelected = path.path.string() == door->sceneFilePath;
                if (ImGui::Selectable(path.name.c_str())) door->sceneFilePath = path.path.string();
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        const auto& preview = door->unlockedTexID.c_str();
        if (ImGui::BeginCombo("Unlocked Door Texture ID", preview)) {
            for (const auto& option : resourceManager.getTextures()) {
                bool isSelected = option.first == door->unlockedTexID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) {
                    door->unlockedTexID = option.first;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }    
        char doorKeyIdBuffer[128];
        std::strncpy(doorKeyIdBuffer, door->keyId.c_str(), sizeof(doorKeyIdBuffer));
        doorKeyIdBuffer[sizeof(doorKeyIdBuffer) - 1] = '\0';
        if (ImGui::InputText("Door Key ID", doorKeyIdBuffer, sizeof(doorKeyIdBuffer))) {
            door->keyId = doorKeyIdBuffer;
        }
        ImGui::Checkbox("Locked", &door->isLocked);
        
    }

    auto key = selectedEntity->getComponent<Key>();
    if (key) {
        ImGui::Separator();
        ImGui::Text("Key");
        char keyIdBuffer[128];
        std::strncpy(keyIdBuffer, key->keyId.c_str(), sizeof(keyIdBuffer));
        keyIdBuffer[sizeof(keyIdBuffer) - 1] = '\0';
        if (ImGui::InputText("Key ID", keyIdBuffer, sizeof(keyIdBuffer))) {
            key->keyId = keyIdBuffer;
        }
    }

    auto switchComponent = selectedEntity->getComponent<Switch>();
    if (switchComponent) {
        ImGui::Separator();
        ImGui::Text("Switch");
        ImGui::Checkbox("Activated", &switchComponent->activated);
        const auto& activatedPreview = switchComponent->activatedTextureID.c_str();
        if (ImGui::BeginCombo("Activated Texture ID", activatedPreview)) {
            for (const auto& option : resourceManager.getTextures()) {
                bool isSelected = option.first == switchComponent->activatedTextureID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) {
                    switchComponent->activatedTextureID = option.first;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        const auto& deactivatedPreview = switchComponent->deactivatedTextureID.c_str();
        if (ImGui::BeginCombo("Deactivated Texture ID", deactivatedPreview)) {
            for (const auto& option : resourceManager.getTextures()) {
                bool isSelected = option.first == switchComponent->deactivatedTextureID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) {
                    switchComponent->deactivatedTextureID = option.first;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    auto powerUp = selectedEntity->getComponent<PowerUp>();
    if (powerUp) {
        ImGui::Separator();
        ImGui::Text("PowerUp");
        ImGui::InputFloat("Velocity Boost", &powerUp->velocityBoost);
        ImGui::InputFloat("Move Speed Boost", &powerUp->moveSpeedBoost);
        ImGui::InputFloat("PowerUp Duration", &powerUp->powerUpDuration);
        ImGui::Text("PowerUp Time Left: %.2f", powerUp->powerUpTimeLeft);

        const char* powerUpTypeNames[] = {"Shotgun", "VelocityBoost", "MoveSpeed"};
        int powerUpType = static_cast<int>(powerUp->powerUpType);
        if (ImGui::Combo("PowerUp Type", &powerUpType, powerUpTypeNames, IM_ARRAYSIZE(powerUpTypeNames))) {
            powerUp->powerUpType = static_cast<PowerUpType>(powerUpType);
        }

        const char* selectionMethodNames[] = {"Random", "Fixed"};
        int selectionMethod = static_cast<int>(powerUp->selectionMethod);
        if (ImGui::Combo("Selection Method", &selectionMethod, selectionMethodNames, IM_ARRAYSIZE(selectionMethodNames))) {
            powerUp->selectionMethod = static_cast<SelectionMethod>(selectionMethod);
        }
    }

    auto popText = selectedEntity->getComponent<PopText>();
    if (popText) {
        ImGui::Separator();
        ImGui::Text("PopText");
        char popTextBuffer[512];
        std::strncpy(popTextBuffer, popText->textString.c_str(), sizeof(popTextBuffer));
        popTextBuffer[sizeof(popTextBuffer) - 1] = '\0';
        if (ImGui::InputTextMultiline("PopText String", popTextBuffer, sizeof(popTextBuffer))) {
            popText->textString = popTextBuffer;
            popText->applyProperties();
        }
        if (ImGui::BeginCombo("PopText Font ID", popText->fontID.c_str())) {
            for (const auto& option : resourceManager.getFonts()) {
                bool isSelected = option.first == popText->fontID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) {
                    popText->fontID = option.first;
                    popText->text->text->setFont(resourceManager.getFont(popText->fontID));
                    popText->applyProperties();
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::DragFloat2("PopText Offset", &popText->offset.x, 1.0f)) {
            popText->applyProperties();
        }
        int popTextSize = static_cast<int>(popText->size);
        if (ImGui::DragInt("PopText Character Size", &popTextSize, 1.0f, 1, 300)) {
            popText->size = static_cast<unsigned int>(popTextSize);
            popText->applyProperties();
        }
        if (ImGui::BeginCombo("PopText Style", getTextStyleLabel(popText->style))) {
            if (ImGui::Selectable("Regular", popText->style == sf::Text::Style::Regular)) popText->style = sf::Text::Style::Regular;
            if (ImGui::Selectable("Bold", popText->style == sf::Text::Style::Bold)) popText->style = sf::Text::Style::Bold;
            if (ImGui::Selectable("Italic", popText->style == sf::Text::Style::Italic)) popText->style = sf::Text::Style::Italic;
            if (ImGui::Selectable("Underlined", popText->style == sf::Text::Style::Underlined)) popText->style = sf::Text::Style::Underlined;
            if (ImGui::Selectable("StrikeThrough", popText->style == sf::Text::Style::StrikeThrough)) popText->style = sf::Text::Style::StrikeThrough;
            ImGui::EndCombo();
            popText->applyProperties();
        }
        float popTextColor[4] = {
            popText->color.r / 255.0f,
            popText->color.g / 255.0f,
            popText->color.b / 255.0f,
            popText->color.a / 255.0f
        };
        if (ImGui::ColorEdit4("PopText Fill Color", popTextColor)) {
            popText->color = sf::Color(
                static_cast<std::uint8_t>(popTextColor[0] * 255.0f),
                static_cast<std::uint8_t>(popTextColor[1] * 255.0f),
                static_cast<std::uint8_t>(popTextColor[2] * 255.0f),
                static_cast<std::uint8_t>(popTextColor[3] * 255.0f)
            );
            popText->applyProperties();
        }
        if (ImGui::DragFloat("PopText Life Time Duration", &popText->lifeTimeDuration, 0.1f, 0.0f)) {
            popText->applyProperties();
        }
    }

    auto loot = selectedEntity->getComponent<Loot>();
    if (loot) {
        ImGui::Separator();
        ImGui::Text("Loot");
        ImGui::DragInt("Gold", &loot->gold, 1.0f, 0);
        ImGui::DragInt("Score", &loot->score, 1.0f, 0);
        ImGui::DragFloat("Experience", &loot->experience, 1.0f, 0.0f);
    }

    auto script = selectedEntity->getComponent<Script>();
    if (script) {
        ImGui::Separator();
        ImGui::Text("Script");
        ImGui::Text("Started: %s", script->started ? "true" : "false");
        ImGui::Text("Rules: %zu", script->rules.size());
        ImGui::Text("Script Name: %s", script->fileName.c_str());
        ImGui::Text("Script File Path: %s", script->filePath.c_str());
        if (ImGui::Button("Edit Linked Script")) {
            editor.SetText(getJsonScriptAsString(script->filePath));
            currentFileName = script->fileName;
            currentFilePath = script->filePath;
            openScriptEditor = true;
        }
        if (ImGui::Button("Link Different Script")) {
            createScriptPopupOpen = true;
        }
    }

    auto text = selectedEntity->getComponent<Text>();
    if (text) {
        ImGui::Separator();
        ImGui::Text("Text");
        char textBuffer[512];
        std::strncpy(textBuffer, text->textString.c_str(), sizeof(textBuffer));
        textBuffer[sizeof(textBuffer) - 1] = '\0';
        if (ImGui::InputTextMultiline("Text String", textBuffer, sizeof(textBuffer))) {
            text->textString = textBuffer;
            text->applyProperties();
        }
        if (ImGui::BeginCombo("Font ID", text->fontID.c_str())) {
            for (const auto& option : resourceManager.getFonts()) {
                bool isSelected = option.first == text->fontID;
                if (ImGui::Selectable(option.first.c_str(), isSelected)) {
                    text->fontID = option.first;
                    text->text->setFont(resourceManager.getFont(text->fontID));
                    text->applyProperties();
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        int characterSize = static_cast<int>(text->characterSize);
        if (ImGui::DragInt("Character Size", &characterSize, 1.0f, 1, 300)) {
            text->characterSize = static_cast<unsigned int>(characterSize);
            text->applyProperties();
        }
        drawTextStyleCombo("Text Style", *text);
        if (ImGui::Checkbox("Link Game Value", &text->linkGameValue)) {
            text->applyProperties();
        }
        ImGui::Checkbox("Position Relative To Viewport", &text->positionRelativeToViewport);
        if (text->linkGameValue) {
            drawGameValueCombo("Game Value", *text);
        }
        float fillColor[4] = {
            text->fillColor.r / 255.0f,
            text->fillColor.g / 255.0f,
            text->fillColor.b / 255.0f,
            text->fillColor.a / 255.0f
        };
        if (ImGui::ColorEdit4("Fill Color", fillColor)) {
            text->fillColor = sf::Color(
                static_cast<std::uint8_t>(fillColor[0] * 255.0f),
                static_cast<std::uint8_t>(fillColor[1] * 255.0f),
                static_cast<std::uint8_t>(fillColor[2] * 255.0f),
                static_cast<std::uint8_t>(fillColor[3] * 255.0f)
            );
            text->applyProperties();
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Add Component")) {
        componentPopupOpen = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rename Entity")) {
        renameEntityPopupOpen = true;
    }
    if (renameEntityPopupOpen) {
        buildRenameEntityPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Duplicate Entity")) {
        duplicateSelectedEntity(scene, resourceManager);
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Component")) {
        removeComponentPopupOpen = true;
    }
    if (removeComponentPopupOpen) {
        buildRemoveComponentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Entity")) {
        scene.destroyEntity(selectedEntity);
        selectedEntity = nullptr;
    }
    ImGui::Separator();
    if (ImGui::Button("Close")) {
        selectedEntity = nullptr;
    }
    
    ImGui::End();
}
