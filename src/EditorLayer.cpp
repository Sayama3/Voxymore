//
// Created by ianpo on 21/08/2023.
//

#include "Voxymore/Editor/EditorLayer.hpp"

namespace Voxymore::Editor {
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_Camera(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight())
    {
        VXM_PROFILE_FUNCTION();
        Application::Get().GetWindow().SetCursorState(m_Camera.IsEnable() ? CursorState::Locked : CursorState::None);
        const Window& window = Application::Get().GetWindow();


        m_VertexArray = VertexArray::Create();

        float cubeVertices [(8 * 3) + (8 * 4)] = {
                -0.5f, -0.5f, +0.5f,   0.8f, 0.1f, 0.7f, 1.0f,
                +0.5f, -0.5f, +0.5f,   0.1f, 0.7f, 0.8f, 1.0f,
                +0.5f, +0.5f, +0.5f,   0.8f, 0.8f, 0.8f, 1.0f,
                -0.5f, +0.5f, +0.5f,   0.1f, 0.1f, 0.1f, 1.0f,

                -0.5f, -0.5f, -0.5f,   0.8, 0.2f, 0.3f, 1.0f,
                +0.5f, -0.5f, -0.5f,   0.2f, 0.3f, 0.8f, 1.0f,
                +0.5f, +0.5f, -0.5f,   0.3f, 0.8f, 0.2f, 1.0f,
                -0.5f, +0.5f, -0.5f,   0.7f, 0.8f, 0.1f, 1.0f,
        };
        m_VertexBuffer = VertexBuffer::Create(sizeof(cubeVertices), cubeVertices);
        BufferLayout layout = {
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4, "a_Color"},
        };
        m_VertexBuffer->SetLayout(layout);


        m_VertexArray->AddVertexBuffer(m_VertexBuffer);


        uint32_t cubeIndices[2 * 3 * 6] = {
                0,1,2,
                0,2,3,
                1,5,6,
                1,6,2,
                5,4,7,
                5,7,6,
                4,0,3,
                4,3,7,
                4,5,1,
                4,1,0,
                3,2,6,
                3,6,7,
        };

        m_IndexBuffer = IndexBuffer::Create(std::size(cubeIndices), cubeIndices);

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_SquareVertexArray = VertexArray::Create();
        float squareVertices[5 * 4] = {
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };
        m_SquareVertexBuffer = VertexBuffer::Create(sizeof(squareVertices), squareVertices);
        BufferLayout squareLayout = {
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float2, "a_TexCoord"},
        };
        m_SquareVertexBuffer->SetLayout(squareLayout);
        m_SquareVertexArray->AddVertexBuffer(m_SquareVertexBuffer);
        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        m_SquareIndexBuffer = IndexBuffer::Create(std::size(squareIndices), squareIndices);
        m_SquareVertexArray->SetIndexBuffer(m_SquareIndexBuffer);

        VXM_INFO("Creat FlatColor Shader");
        m_Shader = Shader::Create("assets/shaders/FlatColor.glsl");
        VXM_INFO("Creat Texture Shader");
        m_TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");

        m_Texture = Texture2D::Create("assets/textures/texture_checker.png");
//        std::dynamic_pointer_cast<OpenGLShader>(m_TextureShader)->Bind();
//        std::dynamic_pointer_cast<OpenGLShader>(m_TextureShader)->SetUniformInt("u_Texture", 0);
//        std::dynamic_pointer_cast<OpenGLShader>(m_TextureShader)->Unbind();
    }

    bool EditorLayer::UpdateCameraPositionPressed(KeyPressedEvent& event) {
        VXM_PROFILE_FUNCTION();
        if (event.GetKeyCode() == KeyCode::KEY_ESCAPE && event.GetRepeatCount() == 0) {
            VXM_CORE_INFO("Press KEY ESCAPE.");
            m_Camera.SetEnable(!m_Camera.IsEnable());
            Application::Get().GetWindow().SetCursorState(m_Camera.IsEnable() ? CursorState::Locked : CursorState::None);
        }
        return false;
    }

    void EditorLayer::OnUpdate(TimeStep timeStep) {
        VXM_PROFILE_FUNCTION();

        m_Camera.OnUpdate(timeStep);

        {
            VXM_PROFILE_SCOPE("Rendering Preparation.");
            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
            RenderCommand::Clear();
        }

        {
            VXM_PROFILE_SCOPE("Rendering Scene");
            Renderer::BeginScene(m_Camera.GetCamera());

            m_Texture->Bind();
            Renderer::Submit(m_TextureShader, m_SquareVertexArray);
            Renderer::Submit(m_Shader, m_VertexArray, Math::TRS(modelPos, glm::quat(glm::radians(modelRot)), modelScale));

            Renderer::EndScene();
            m_Framebuffer->Unbind();
        }
    }

    void EditorLayer::OnImGuiRender() {
        VXM_PROFILE_FUNCTION();
        {
            VXM_PROFILE_SCOPE("EditorLayer::OnImGuiRender -> Dockspace");
            // READ THIS !!!
            // TL;DR; this demo is more complicated than what most users you would normally use.
            // If we remove all options we are showcasing, this demo would become:
            //     void ShowExampleAppDockSpace()
            //     {
            //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            //     }
            // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
            // In this specific demo, we are not using DockSpaceOverViewport() because:
            // - (1) we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
            // - (2) we allow the host window to have padding (when opt_padding == true)
            // - (3) we expose many flags and need a way to have them visible.
            // - (4) we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport()
            //      in your code, but we don't here because we allow the window to be floating)

            static bool dockspaceOpen = false;
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            else
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
                ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
                ImGui::SameLine(0.0f, 0.0f);
                if (ImGui::SmallButton("click here"))
                    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    if(ImGui::MenuItem("Fullscreen", ""))
                    {
                        Application::Get().Close();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
        }

        {
            VXM_PROFILE_SCOPE("EditorLayer::OnImGuiRender -> Model Component drawing");
            ImGui::Begin("Model Component");

            ImGui::DragFloat3("Position", glm::value_ptr(modelPos));
            ImGui::DragFloat3("Rotation", glm::value_ptr(modelRot));
            ImGui::DragFloat3("Scale", glm::value_ptr(modelScale));

            ImGui::End();
        }

        {
            VXM_PROFILE_SCOPE("EditorLayer::OnImGuiRender -> Rendering");
            ImGui::Begin("Rendering");
            uint32_t texID = m_Framebuffer->GetColorAttachmentRendererID();
            static float scale = 1.0f;
            ImGui::SliderFloat("ImageScale", &scale, 0.0f, 1.0f);
            ImGui::Image((void*)texID, ImVec2(m_Framebuffer->GetSpecification().Width * scale, m_Framebuffer->GetSpecification().Height * scale), ImVec2{ 0,1 }, ImVec2{1,0});
            ImGui::End();
        }

    }

    void EditorLayer::OnEvent(Event& event) {
        VXM_PROFILE_FUNCTION();
        m_Camera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::UpdateCameraPositionPressed, std::placeholders::_1));
    }

    void EditorLayer::OnAttach() {
        VXM_PROFILE_FUNCTION();
        FramebufferSpecification specification(1280, 720);
        m_Framebuffer = Framebuffer::Create(specification);
    }
    void EditorLayer::OnDetach() {
        VXM_PROFILE_FUNCTION();

    }
} // Voxymore
// Editor