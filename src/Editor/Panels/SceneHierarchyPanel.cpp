//
// Created by ianpo on 25/08/2023.
//

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "Voxymore/Editor/Panels/SceneHierarchyPanel.hpp"


namespace Voxymore::Editor {
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene) : m_Context(scene)
    {
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene> &scene)
    {
        m_PropertyPanel.m_SelectedEntity = {};
        m_Context = scene;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Hierarchy");


        {
            std::string idstr = std::to_string(m_Context->m_ID);
            ImGui::Text("%s", idstr.c_str());
        }

        ImGui::SameLine();

        {
            const size_t bufferSize = 256;
            char buffer[bufferSize];
            buffer[bufferSize - 1] = 0;
            std::strncpy(buffer, m_Context->m_Name.c_str(), sizeof(buffer) - 1);
            if (ImGui::InputText("##SceneName", buffer, bufferSize)) {
                m_Context->m_Name = buffer;
            }
        }

        ImGui::Separator();

        auto transformView = m_Context->m_Registry.view<TagComponent>();
        for (auto entity : transformView)
        {
            DrawEntityNode({entity, m_Context.get()});
        }

        if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_PropertyPanel.m_SelectedEntity = {};
        }
        else if(ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused())
        {
            m_PropertyPanel.m_SelectedEntity = {};
        }

        // Right click on blank space
        if(ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if(ImGui::MenuItem("Create Empty Entity"))
            {
                m_Context->CreateEntity();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        m_PropertyPanel.OnImGuiRender();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        void* EntityID = (void*)(uint64_t)(uint32_t)entity;
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if(entity == m_PropertyPanel.m_SelectedEntity) flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(EntityID, flags, tag.c_str());
        //TODO: change selected entity on selection changed.
        if(ImGui::IsItemClicked() || ImGui::IsItemActivated())
        {
            m_PropertyPanel.m_SelectedEntity = entity;
        }

        bool deleteEntity = false;
        // Right click on blank space
        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem(std::string("Delete '").append(tag).append("'").c_str()))
            {
                deleteEntity = true;
            }
            ImGui::EndPopup();
        }

        if(open)
        {
            //TODO: display the children.
            ImGui::TreePop();
        }

        if(deleteEntity)
        {
            if(m_PropertyPanel.m_SelectedEntity == entity) m_PropertyPanel.m_SelectedEntity = {};
            m_Context->DestroyEntity(entity);
        }
    }
} // Voxymore
// Editor