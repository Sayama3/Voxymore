//
// Created by ianpo on 21/08/2023.
//

#pragma once
#include "Voxymore/Voxymore.hpp"
#include "Voxymore/Renderer/Framebuffer.hpp"
#include "Voxymore/Renderer/EditorCamera.hpp"
#include "Voxymore/Core/PerspectiveCameraController.hpp"
#include "Voxymore/Editor/Panels/SceneHierarchyPanel.hpp"
#include "Voxymore/Editor/Panels/PropertyPanel.hpp"

using namespace Voxymore::Core;

namespace Voxymore::Editor {

    // Fetched from ImGuizmo
    enum class GizmoOperation
    {
        NONE             = 0,
        TRANSLATE_X      = (1u << 0),
        TRANSLATE_Y      = (1u << 1),
        TRANSLATE_Z      = (1u << 2),
        ROTATE_X         = (1u << 3),
        ROTATE_Y         = (1u << 4),
        ROTATE_Z         = (1u << 5),
        ROTATE_SCREEN    = (1u << 6),
        SCALE_X          = (1u << 7),
        SCALE_Y          = (1u << 8),
        SCALE_Z          = (1u << 9),
        BOUNDS           = (1u << 10),
        SCALE_XU         = (1u << 11),
        SCALE_YU         = (1u << 12),
        SCALE_ZU         = (1u << 13),

        TRANSLATE = TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z,
        ROTATE = ROTATE_X | ROTATE_Y | ROTATE_Z | ROTATE_SCREEN,
        SCALE = SCALE_X | SCALE_Y | SCALE_Z,
        SCALEU = SCALE_XU | SCALE_YU | SCALE_ZU, // universal
        UNIVERSAL = TRANSLATE | ROTATE | SCALEU

    };
    inline GizmoOperation operator|(GizmoOperation lhs, GizmoOperation rhs)
    {
        return static_cast<GizmoOperation>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }
    inline GizmoOperation operator&(GizmoOperation lhs, GizmoOperation rhs)
    {
        return static_cast<GizmoOperation>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }
    enum class GizmoMode
    {
        LOCAL,
        WORLD
    };

    class EditorLayer : public Layer {
    private:
        Ref<Shader> m_Shader;
        Ref<Material> m_Material;
        Ref<Texture2D> m_Texture;
        Ref<Shader> m_TextureShader;
        Ref<Material> m_TextureMaterial;

        Ref<VertexArray> m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

        Ref<VertexArray> m_SquareVertexArray;
        Ref<VertexBuffer> m_SquareVertexBuffer;
        Ref<IndexBuffer> m_SquareIndexBuffer;

        Ref<Framebuffer> m_Framebuffer;
    private:
        std::array<glm::vec2, 2> m_ViewportBounds;
    private:
        SceneHierarchyPanel m_SceneHierarchyPanel;
        Ref<Scene> m_ActiveScene;
        Entity m_CubeEntity;
        Entity m_TextureEntity;
        Entity m_ActiveCamera;
        Entity m_HoveredEntity;
        std::string m_FilePath;

        EditorCamera m_EditorCamera;
    private:
        glm::uvec2 m_ViewportSize = glm::uvec2(0);
        bool m_ViewportFocused;
        bool m_ViewportHovered;
        bool m_CameraEnable = false;
    private: // Gizmo
        GizmoOperation m_GizmoOperation = GizmoOperation::TRANSLATE;
        GizmoMode m_GizmoMode = GizmoMode::LOCAL;
        float m_GizmoTranslationSnapValue = 0.5f;
        float m_GizmoRotationSnapValue = 45.0f;
        float m_GizmoScaleSnapValue = 0.5f;
    public:
        EditorLayer();

        virtual void OnUpdate(TimeStep timeStep) override;

        virtual void OnImGuiRender() override;

        virtual void OnEvent(Event& event);

        virtual void OnAttach() override;
        virtual void OnDetach() override;
    private:
        void DrawImGuiViewport();
        void RenderDockspace();
        void DrawGizmos();
        void DrawGizmosWindow();

        void RenderMenuBar();
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMousePressed(MouseButtonPressedEvent& e);

        void CreateNewScene();
        void SaveSceneAs();
        void SaveScene();
        void OpenScene();
    };

} // Voxymore
// Editor
