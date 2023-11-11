//
// Created by ianpo on 25/08/2023.
//

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "Voxymore/Editor/Panels/PropertyPanel.hpp"
#include "Voxymore/Scene/CustomComponent.hpp"
#include "imgui_internal.h"
#include <cstring>


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
                std::strncpy(buffer, tag.c_str(), sizeof(buffer));
                if(ImGui::InputText("##Tag", buffer, bufferSize))
                {
                    tag = std::string(buffer);
                }
            }

            ImGui::SameLine();
            ImGui::PushItemWidth(-1);

            if(ImGui::Button("Add Component")) {
                ImGui::OpenPopup("AddComponentPopup");
            }

            ImGui::PopItemWidth();
            if(ImGui::BeginPopup("AddComponentPopup"))
            {
                if(!m_SelectedEntity.HasComponent<CameraComponent>() && ImGui::MenuItem("Camera"))
                {
                    auto& newCam = m_SelectedEntity.AddComponent<CameraComponent>();
                    ImGui::CloseCurrentPopup();
                }

                if(!m_SelectedEntity.HasComponent<MeshComponent>() && ImGui::MenuItem("Mesh Component"))
                {
                    m_SelectedEntity.AddComponent<MeshComponent>();
                    ImGui::CloseCurrentPopup();
                }

                for (const ComponentChecker& cc : ComponentManager::GetComponents())
                {
                    if(!cc.HasComponent(m_SelectedEntity) && ImGui::MenuItem(cc.ComponentName.c_str()))
                    {
                        cc.AddComponent(m_SelectedEntity);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }

            DrawComponents();
        }
        else
        {
            ImGui::Text("No entity selected.");
        }
        ImGui::End();
    }

    bool PropertyPanel::DrawVec3ControlAdvanced(const std::string& label, glm::vec3& values, float step, float min, float max, const std::string& format, float resetValue, float columnWidth) {
        // ==================== START ====================
        bool changed = false;
        ImFont* boldFont = Application::Get().GetImGuiLayer()->GetFont(FontType::Bold);
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
        ImGui::PushFont(boldFont);
        if(ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
            changed |= true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, step, min, max, format.c_str());
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // ==================== Y ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.9f, 0.4f, 1.0f));
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
            changed |= true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, step, min, max, format.c_str());
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // ==================== Z ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.45f, 1.0f, 1.0f));
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
            changed |= true;
        }
        ImGui::PopFont();
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
    bool PropertyPanel::DrawVec2ControlAdvanced(const std::string& label, glm::vec2& values, float step, float min, float max, const std::string& format, float resetValue, float columnWidth) {
        // ==================== START ====================
        bool changed = false;
        ImFont* boldFont = Application::Get().GetImGuiLayer()->GetFont(FontType::Bold);
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize(lineHeight + 3.0f, lineHeight);

        // ==================== X ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.25f, 1.0f));
        ImGui::PushFont(boldFont);
        if(ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
            changed |= true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, step, min, max, format.c_str());
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // ==================== Y ====================
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.9f, 0.4f, 1.0f));
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
            changed |= true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, step, min, max, format.c_str());
        ImGui::PopItemWidth();

        // ==================== END ====================
        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::PopID();
        return changed;
    }

    void PropertyPanel::DrawComponents()
    {
        DrawComponent<TransformComponent>("Transform Component", m_SelectedEntity, [](TransformComponent& transformComponent){
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
        }, false);

        DrawComponent<MeshComponent>("Mesh Component", m_SelectedEntity, [](MeshComponent& meshComponent){
            ImGui::Text("Cannot modify the mesh component yet.");
        });

        DrawComponent<CameraComponent>("Camera Component", m_SelectedEntity, [](CameraComponent& cameraComponent){
            ImGui::Checkbox("Primary", &cameraComponent.Primary);
            ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
            if(cameraComponent.FixedAspectRatio)
            {
                ImGui::SameLine();
                float ar = cameraComponent.Camera.GetAspectRatio();
                if(ImGui::DragFloat("##AspectRatio", &ar, 0.001f))
                {
                    cameraComponent.Camera.SetAspectRatio(ar);
                }
            }

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

        for (const ComponentChecker& cc : ComponentManager::GetComponents())
        {
            DrawComponent(m_SelectedEntity, cc);
        }
    }

    void PropertyPanel::DrawComponent(Entity entity, const ComponentChecker& component, bool canBeDeleted)
        {
            const ImGuiTreeNodeFlags treeNodeFlags =
                    ImGuiTreeNodeFlags_DefaultOpen |
                    ImGuiTreeNodeFlags_AllowItemOverlap |
                    ImGuiTreeNodeFlags_SpanAvailWidth |
                    ImGuiTreeNodeFlags_FramePadding |
                    ImGuiTreeNodeFlags_Framed;
            if (component.HasComponent(entity))
            {
                ImVec2 contentAvailable = ImGui::GetContentRegionAvail();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::Separator();
                bool open = ImGui::TreeNodeEx((void*)component.ComponentHash, treeNodeFlags, "%s", component.ComponentName.c_str());
                ImGui::PopStyleVar();

                bool removeComponent = false;
                ImGui::SameLine(contentAvailable.x - lineHeight * 0.5f);
                if(ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }
                if(ImGui::BeginPopup("ComponentSettings"))
                {
                    if(canBeDeleted && ImGui::MenuItem("Remove Component"))
                    {
                        removeComponent = true;
                    }
                    ImGui::EndPopup();
                }

                if(open)
                {
                    component.OnImGuiRender(entity);
                    ImGui::TreePop();
                }

                if(removeComponent)
                {
                    component.RemoveComponent(entity);
                }
            }

        }



} // Voxymore
// Editor