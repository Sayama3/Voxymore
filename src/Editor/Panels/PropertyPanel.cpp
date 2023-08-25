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

            DrawComponent<TransformComponent>("Transform Component", [](TransformComponent& transformComponent){
                ImGui::DragFloat3("Position", glm::value_ptr(transformComponent.Position), 0.1f);

                glm::vec3 rot = glm::degrees(glm::eulerAngles(transformComponent.Rotation));
                if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.1f))
                    transformComponent.Rotation = glm::quat(glm::radians(rot));

                ImGui::DragFloat3("Scale", glm::value_ptr(transformComponent.Scale), 0.1f);

            });

            DrawComponent<MeshComponent>("Mesh Component", [](MeshComponent& meshComponent){
                ImGui::Text("Cannot modify the mesh component yet.");
            });

            DrawComponent<CameraComponent>("Camera Component", [](CameraComponent& cameraComponent){
                ImGui::Checkbox("Primary", &cameraComponent.Primary);
                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);

                const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
                uint8_t isOrthographic = cameraComponent.Camera.IsOrthographic();
                const char* currentProjectionTypeString = projectionTypeStrings[isOrthographic];
                if(ImGui::BeginCombo("Projection",currentProjectionTypeString))
                {
                    for (int i = 0; i < 2; ++i) {
                        bool isSelected = isOrthographic == i;
                        if(ImGui::Selectable(projectionTypeStrings[i], isSelected))
                        {
                            cameraComponent.Camera.SwitchToOrthographic(i);
                        }

                        if(isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (isOrthographic)
                {
                    float size = cameraComponent.Camera.GetOrthographicSize();
                    float orthoNearClip = cameraComponent.Camera.GetOrthographicNear();
                    float orthoFarClip = cameraComponent.Camera.GetOrthographicFar();

                    bool changed = false;
                    changed |= ImGui::DragFloat("Size", &size, 0.1f, 0.1f);
                    changed |= ImGui::DragFloat("Near", &orthoNearClip, 0.01f);
                    changed |= ImGui::DragFloat("Far", &orthoFarClip, 0.01f);
                    if(changed) cameraComponent.Camera.SetOrthographic(size, orthoNearClip, orthoFarClip);
                }
                else
                {
                    float fov = glm::degrees(cameraComponent.Camera.GetPerspectiveVerticalFOV());
                    float perspectiveNearClip = cameraComponent.Camera.GetPerspectiveNear();
                    float perspectiveFarClip = cameraComponent.Camera.GetPerspectiveFar();

                    bool changed = false;
                    changed |= ImGui::DragFloat("Vertical Fov", &fov, 1.0f, 1.0f, 179.0f);
                    changed |= ImGui::DragFloat("Near", &perspectiveNearClip, 0.01f);
                    changed |= ImGui::DragFloat("Far", &perspectiveFarClip);
                    if(changed) cameraComponent.Camera.SetPerspective(glm::radians(fov), perspectiveNearClip, perspectiveFarClip);
                }
            });

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