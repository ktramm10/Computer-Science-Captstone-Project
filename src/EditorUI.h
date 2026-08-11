#include <imgui.h>
#include <imgui-SFML.h>
#include <filesystem>
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneSerializer.h"
#include "SceneLoader.h"
#include "TextEditor.h"
#include "AIPromptBridge.h"

class EditorUI {
    public:
    EditorUI() = default;
    TextEditor editor;
    void buildUI(Scene& scene, ResourceManager& resourceManager, SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, bool& isPlaying, sf::View& camera, sf::Music& musicPlayer);
    void handleDragMove(sf::RenderWindow& window, bool isPlaying);
    const bool getIsSceneLoaded() { return isSceneLoaded; }
    private:
    void buildSceneSelect(Scene& scene, SceneLoader& sceneLoader, ResourceManager& resourceManager, sf::View& camera);
    void buildSceneHierarchy(Scene& scene);
    void buildEntityInspector(Scene& scene, ResourceManager& resourceManager);
    void buildEntityCreator(Scene& scene);
    void duplicateSelectedEntity(Scene& scene, ResourceManager& resourceManager);
    void buildRenameEntityPopup();
    void buildAddComponentPopup(Scene& scene, ResourceManager& ResourceManager);
    void buildRemoveComponentPopup();
    void buildScenePanel(Scene& scene, ResourceManager& resourceManager, SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, bool& isPlaying, sf::View& camera, sf::Music& musicPlayer);
    void buildSaveSceneAsPanel(SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, Scene& scene, ResourceManager& resourceManager, sf::View& camera);
    void buildNewScenePanel(SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, Scene& scene, ResourceManager& resourceManager, sf::View& camera);
    void buildToolBarPanel(SceneSerializer& sceneSerializer, SceneLoader& sceneLoader, Scene& scene, ResourceManager& resourceManager, bool& isPlaying, sf::View& camera, sf::Music& musicPlayer);
    void buildAIPromptPanel(Scene& scene, ResourceManager& resourceManager, SceneLoader& sceneLoader, sf::View& camera, SceneSerializer& sceneSerializer);
    void buildScriptEditor();
    void buildCreateScriptPopup();
    void openScriptInEditor(std::string fileName, std::string filePath);
    std::shared_ptr<Entity> selectedEntity = nullptr;
    std::shared_ptr<Component> selectedComponent = nullptr;
    void buildHiddenEditorPanel();
    
    bool entityCreatorOpen = false;
    bool componentPopupOpen = false;
    bool removeComponentPopupOpen = false;
    bool isSceneLoaded = false;
    bool saveSceneAsOpen = false;
    bool newSceneOpen = false;
    bool dragMoveEnabled = false;
    bool renameEntityPopupOpen = false;
    bool createScriptPopupOpen = false;
    bool openScriptEditor = false;
    bool hideEditorPanels = false;
    bool openAIPromptPanel = false;
    std::string aiPromptStatus;
    bool hasAIPromptResult = false;
    AIPromptResult lastAIPromptResult;
    AIPromptBridge aiPromptBridge;
    std::string currentFileName = "none";
    std::string currentFilePath = "null";
    
    

    
};