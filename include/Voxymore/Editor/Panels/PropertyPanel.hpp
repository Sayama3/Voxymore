//
// Created by ianpo on 25/08/2023.
//

#pragma once

#include <Voxymore/Voxymore.hpp>
#include <imgui.h>

using namespace Voxymore::Core;

namespace Voxymore::Editor {

    class PropertyPanel
    {
    public:
        void OnImGuiRender();
    private:
        template<typename T>
        inline void DrawComponent(const char* name, void(*OnDrawComponent)(T& component))
        {
            if (m_SelectedEntity.HasComponent<T>())
            {
                if(ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "%s", name))
                {
                    OnDrawComponent(m_SelectedEntity.GetComponent<T>());
                    ImGui::TreePop();
                }
            }
        }
    private:
        Entity m_SelectedEntity;
        friend class SceneHierarchyPanel;
    };

} // Voxymore
// Editor
