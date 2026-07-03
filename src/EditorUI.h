#include <imgui.h>
#include <imgui-SFML.h>
#include "Scene.h"

class EditorUI {
    public:
    EditorUI() = default;
    void buildUI(Scene& scene);
    void buildSceneHierarchy(Scene& scene);
    void buildEntityInspector();
    std::shared_ptr<Entity> selectedEntity = nullptr;
};