//
// Created by ianpo on 25/08/2023.
//

#include "Voxymore/Editor/Panels/PropertyPanel.hpp"
#include "imgui_internal.h"


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
                glm::vec3 pos = transformComponent.GetPosition();
                if(DrawVec3Control("Position", pos))
                {
                    transformComponent.SetPosition(pos);
                }

                glm::vec3 rot = glm::degrees(transformComponent.GetEulerRotation());
                if (DrawVec3Control("Rotation", rot))
                {
                    transformComponent.SetEulerRotation(glm::radians(rot));
                }

                glm::vec3 scale = transformComponent.GetScale();
                if(DrawVec3Control("Scale", scale, 1.0f))
                {
                    transformComponent.SetScale(scale);
                }
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

    bool PropertyPanel::DrawVec3ControlAdvanced(const std::string& label, glm::vec3& values, float step, float min, float max, const std::string& format, float resetValue, float columnWidth) {
        // ==================== START ====================
        bool changed = false;
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize(lineHeight + 3.0f, lineHeight);


        // ==================== X ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.25f, 1.0f));
        if(ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
            changed |= true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, step, min, max, format.c_str());
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // ==================== Y ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.9f, 0.4f, 1.0f));
        if(ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
            changed |= true;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, step, min, max, format.c_str());
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // ==================== Z ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.45f, 1.0f, 1.0f));
        if(ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
            changed |= true;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Z", &values.z, step, min, max, format.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();

        // ==================== END ====================
        ImGui::Columns(1);
        ImGui::PopID();
        return changed;
    }
} // Voxymore
// Editor