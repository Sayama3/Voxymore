//
// Created by ianpo on 25/08/2023.
//

#include "Voxymore/Editor/Panels/SceneHierarchyPanel.hpp"


namespace Voxymore::Editor {
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene) : m_Context(scene)
    {
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene> &scene)
    {
        m_Context = scene;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Hierarchy");
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

        ImGui::End();

        m_PropertyPanel.OnImGuiRender();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        void* EntityID = (void*)(uint64_t)(uint32_t)entity;
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

        if(entity == m_PropertyPanel.m_SelectedEntity) flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(EntityID, flags, tag.c_str());
        if(ImGui::IsItemClicked() || ImGui::IsItemActivated())
        {
            m_PropertyPanel.m_SelectedEntity = entity;
        }
        //TODO: Selection Change

        if(open)
        {
            //TODO: display the parents.
            ImGui::TreePop();
        }
    }
} // Voxymore
// Editor