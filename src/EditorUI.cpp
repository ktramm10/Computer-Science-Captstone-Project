#include "EditorUI.h"

void EditorUI::buildUI(Scene& scene)
{
    buildSceneHierarchy(scene);
    buildEntityInspector();
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
    ImGui::End();
}

void EditorUI::buildEntityInspector()
{
    ImGui::Begin("Entity Inspector");
    if (!selectedEntity) {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    ImGui::Text("Selected: %s", selectedEntity->getName().c_str());
    auto transform = selectedEntity->getComponent<Transform>();
    if (transform) {
        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::DragFloat2("Position", &transform->position.x, 1.0f);
        ImGui::DragFloat2("Scale", &transform->scale.x, 1.0f);
        ImGui::DragFloat("Rotation", &transform->rotation, 1.0f);
    }
    ImGui::End();
}
