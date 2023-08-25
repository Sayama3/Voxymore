//
// Created by ianpo on 25/08/2023.
//

#pragma once

#include <Voxymore/Voxymore.hpp>
#include <Voxymore/Scene/Scene.hpp>
#include <Voxymore/Editor/Panels/PropertyPanel.hpp>
#include <imgui.h>

using namespace Voxymore::Core;

namespace Voxymore::Editor {

    class SceneHierarchyPanel
    {
    public:
        inline SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);
        void SetContext(const Ref<Scene>& scene);

        void OnImGuiRender();
    private:
        void DrawEntityNode(Entity entity);
    private:
        Ref<Scene> m_Context;
//        Entity m_SelectionContext;
        PropertyPanel m_PropertyPanel;
    };
} // Voxymore
// Editor
