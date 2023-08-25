//
// Created by ianpo on 25/08/2023.
//

#include "Voxymore/Editor/Panels/PropertyPanel.hpp"


namespace Voxymore::Editor {

    void PropertyPanel::OnImGuiRender()
    {
        ImGui::Begin("Properties");
        if(m_SelectedEntity.IsValid())
        {
            //TODO: Add a remove Component button on each component.
            if (m_SelectedEntity.HasComponent<TagComponent>())
            {
                auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
                const size_t bufferSize = 256;
                char buffer[bufferSize];
                memset(buffer, 0, bufferSize);
                strcpy_s(buffer, tag.c_str());
                if(ImGui::InputText("Tag", buffer, bufferSize))
                {
                    tag = std::string(buffer);
                }
            }

            if (m_SelectedEntity.HasComponent<TransformComponent>())
            {
                if(ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform Component"))
                {
                    auto& transformCamera = m_SelectedEntity.GetComponent<TransformComponent>();
                    ImGui::DragFloat3("Position", glm::value_ptr(transformCamera.Position), 0.1f);

                    glm::vec3 rot = glm::degrees(glm::eulerAngles(transformCamera.Rotation));
                    if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.1f))
                        transformCamera.Rotation = glm::quat(glm::radians(rot));

                    ImGui::DragFloat3("Scale", glm::value_ptr(transformCamera.Scale), 0.1f);
                    ImGui::TreePop();
                }
            }

            if (m_SelectedEntity.HasComponent<MeshComponent>())
            {
                if(ImGui::TreeNodeEx((void*)typeid(MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh Component"))
                {
                    ImGui::Text("Cannot modify the mesh component yet.");
                    ImGui::TreePop();
                }
            }

            if (m_SelectedEntity.HasComponent<CameraComponent>())
            {
                if(ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera Component")) {
                    auto &sceneCamera = m_SelectedEntity.GetComponent<CameraComponent>();

                    ImGui::Checkbox("Primary", &sceneCamera.Primary);
                    ImGui::Checkbox("Fixed Aspect Ratio", &sceneCamera.FixedAspectRatio);

                    const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
                    uint8_t isOrthographic = sceneCamera.Camera.IsOrthographic();
                    const char* currentProjectionTypeString = projectionTypeStrings[isOrthographic];
                    if(ImGui::BeginCombo("Projection",currentProjectionTypeString))
                    {
                        for (int i = 0; i < 2; ++i) {
                            bool isSelected = isOrthographic == i;
                            if(ImGui::Selectable(projectionTypeStrings[i], isSelected))
                            {
                                currentProjectionTypeString = projectionTypeStrings[i];
                                sceneCamera.Camera.SwitchToOrthographic(i);
                            }

                            if(isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

//                    if(ImGui::Checkbox("IsOrthographic", &isOrthographic)) sceneCamera.Camera.SwitchToPerspective(!isOrthographic);

                    if (isOrthographic) {
                        float size = sceneCamera.Camera.GetOrthographicSize();
                        float orthoNearClip = sceneCamera.Camera.GetOrthographicNear();
                        float orthoFarClip = sceneCamera.Camera.GetOrthographicFar();

                        bool changed = false;
                        changed |= ImGui::DragFloat("Size", &size, 0.1f, 0.1f);
                        changed |= ImGui::DragFloat("Near", &orthoNearClip, 0.01f);
                        changed |= ImGui::DragFloat("Far", &orthoFarClip, 0.01f);
                        if(changed) sceneCamera.Camera.SetOrthographic(size, orthoNearClip, orthoFarClip);
                    } else {
                        float fov = glm::degrees(sceneCamera.Camera.GetPerspectiveVerticalFOV());
                        float perspectiveNearClip = sceneCamera.Camera.GetPerspectiveNear();
                        float perspectiveFarClip = sceneCamera.Camera.GetPerspectiveFar();

                        bool changed = false;
                        changed |= ImGui::DragFloat("Vertical Fov", &fov, 1.0f, 1.0f, 179.0f);
                        changed |= ImGui::DragFloat("Near", &perspectiveNearClip, 0.01f);
                        changed |= ImGui::DragFloat("Far", &perspectiveFarClip);
                        if(changed) sceneCamera.Camera.SetPerspective(glm::radians(fov), perspectiveNearClip, perspectiveFarClip);
                    }
                    ImGui::TreePop();
                }
            }

            //TODO: add a AddComponent button
        }
        else
        {
            ImGui::Text("Nothing Selected...");
        }
        ImGui::End();
    }
} // Voxymore
// Editor