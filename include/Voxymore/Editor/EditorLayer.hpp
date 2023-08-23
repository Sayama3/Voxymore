//
// Created by ianpo on 21/08/2023.
//

#pragma once
#include "Voxymore/Voxymore.hpp"
#include "Voxymore/Renderer/Framebuffer.hpp"
#include "Voxymore/Core/PerspectiveCameraController.hpp"

using namespace Voxymore::Core;

namespace Voxymore::Editor {

    class EditorLayer : public Layer {
    private:
        Ref<Shader> m_Shader;
        Ref<Shader> m_TextureShader;
        Ref<Texture2D> m_Texture;

        Ref<VertexArray> m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

        Ref<VertexArray> m_SquareVertexArray;
        Ref<VertexBuffer> m_SquareVertexBuffer;
        Ref<IndexBuffer> m_SquareIndexBuffer;

        Ref<Framebuffer> m_Framebuffer;

        PerspectiveCameraController m_Camera;
    private:
        glm::uvec2 m_ViewportSize = glm::uvec2(0);
        bool m_ViewportFocused;
        bool m_ViewportHovered;
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
    };

} // Voxymore
// Editor
