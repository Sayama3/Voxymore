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
        inline void DrawComponent(const std::string& name, void(*OnDrawComponent)(T& component))
        {
            if (m_SelectedEntity.HasComponent<T>())
            {
                if(ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "%s", name.c_str()))
                {
                    OnDrawComponent(m_SelectedEntity.GetComponent<T>());
                    ImGui::TreePop();
                }
            }
        }

        inline static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
        {
            return DrawVec3ControlAdvanced(label, values, 0.1f, 0.0f, 0.0f, "%.3f", resetValue, columnWidth);
        }
        static bool DrawVec3ControlAdvanced(const std::string& label, glm::vec3& values, float step = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.3f", float resetValue = 0.0f, float columnWidth = 100.0f);
        inline static bool DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f)
        {
            return DrawVec2ControlAdvanced(label, values, 0.1f, 0.0f, 0.0f, "%.3f", resetValue, columnWidth);
        }
        static bool DrawVec2ControlAdvanced(const std::string& label, glm::vec2& values, float step = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.3f", float resetValue = 0.0f, float columnWidth = 100.0f);
    private:
        Entity m_SelectedEntity;
        friend class SceneHierarchyPanel;
    };

} // Voxymore
// Editor
