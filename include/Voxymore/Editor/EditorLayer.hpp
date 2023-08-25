//
// Created by ianpo on 21/08/2023.
//

#pragma once
#include "Voxymore/Voxymore.hpp"
#include "Voxymore/Renderer/Framebuffer.hpp"
#include "Voxymore/Core/PerspectiveCameraController.hpp"
#include "Voxymore/Editor/Panels/SceneHierarchyPanel.hpp"
#include "Voxymore/Editor/Panels/PropertyPanel.hpp"

using namespace Voxymore::Core;

namespace Voxymore::Editor {

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
        SceneHierarchyPanel m_SceneHierarchyPanel;
        PropertyPanel m_PropertyPanel;
        Ref<Scene> m_ActiveScene;
        Entity m_CubeEntity;
        Entity m_TextureEntity;
        Entity m_ActiveCamera;

        PerspectiveCameraController m_Camera;
    private:
        glm::uvec2 m_ViewportSize = glm::uvec2(0);
        bool m_ViewportFocused;
        bool m_ViewportHovered;
        bool m_CameraEnable = false;
    private:
        glm::vec3 m_ModelPos = {0, 0, -2};
        glm::vec3 m_ModelRot = {0, 0, 0};
        glm::vec3 m_ModelScale = {1, 1, 1};
    public:
        EditorLayer();

        virtual void OnUpdate(TimeStep timeStep) override;

        virtual void OnImGuiRender() override;

        virtual void OnEvent(Event& event);

        virtual void OnAttach() override;
        virtual void OnDetach() override;
    private:
        void DrawImGuiViewport();
    };

} // Voxymore
// Editor
