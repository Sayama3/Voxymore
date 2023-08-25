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
        Entity m_SelectedEntity;
        friend class SceneHierarchyPanel;
    };

} // Voxymore
// Editor
