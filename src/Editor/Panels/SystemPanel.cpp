#include "Voxymore/Editor/Panels/SystemPanel.hpp"
#include "Voxymore/Scene/SceneManager.hpp"
#include <imgui.h>
#include <imgui_internal.h>

namespace Voxymore::Editor
{
    void SystemPanel::OnImGuiRender()
    {
        m_SceneNames = SceneManager::GetSceneNames();;
        auto systemNames = SystemManager::GetSystemsName();
        ImGui::Begin("Systems");
        for (const auto& name : systemNames)
        {
            DrawSystem(name);
            ImGui::Separator();
        }
        ImGui::End();
    }

    void SystemPanel::DrawSystem(const std::string &name)
    {
        std::hash<std::string> strHash;
        uint64_t hash = strHash(name);
        const ImGuiTreeNodeFlags treeNodeFlags =
                ImGuiTreeNodeFlags_DefaultOpen |
                ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_FramePadding |
                ImGuiTreeNodeFlags_Framed;

        auto& sysScenes = SystemManager::GetSystemScenes(name);

        ImVec2 contentAvailable = ImGui::GetContentRegionAvail();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)hash, treeNodeFlags, "%s", name.c_str());
        ImGui::PopStyleVar();

        ImGui::SameLine(contentAvailable.x - lineHeight * 0.5f);
        bool enable = SystemManager::IsActive(name);
        if(ImGui::Checkbox("Enable", &enable))
        {
            SystemManager::SetActive(name, enable);
        }

        if(open)
        {
            ImGui::Text("Scenes");
            ImVec2 tableSize = ImGui::GetContentRegionAvail();
            if (ImGui::BeginTable("##ScenesTable", tableSize.x > 0 ? glm::min((int)tableSize.x % 250, (int)m_SceneNames.size()) : 1 , ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
            {
                for (auto &sceneName: m_SceneNames)
                {
                    bool sceneIsActive = std::find(sysScenes.begin(), sysScenes.end(), sceneName) != sysScenes.end();
                    ImGui::TableNextColumn();
                    bool changed = ImGui::Selectable(sceneName.c_str(), &sceneIsActive); // FIXME-TABLE: Selection overlap
                    if (changed) {
                        if (sceneIsActive) SystemManager::AddSceneToSystemIfNotExist(name, sceneName);
                        else SystemManager::RemoveSceneFromSystem(name, sceneName);
                    }
                }
                ImGui::EndTable();
            }

            SystemManager::GetSystem(name)->OnImGuiRender();
            ImGui::TreePop();
        }
    }
}
