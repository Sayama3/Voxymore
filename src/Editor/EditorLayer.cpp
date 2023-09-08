//
// Created by ianpo on 21/08/2023.
//

#include "Voxymore/Editor/EditorLayer.hpp"
#include "Voxymore/Utils/Platform.hpp"

namespace Voxymore::Editor {
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_Camera(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight())
    {
        VXM_PROFILE_FUNCTION();
        Application::Get().GetWindow().SetCursorState(CursorState::None);
        const Window& window = Application::Get().GetWindow();
        m_Camera.SetEnable(false);

        m_ActiveScene = CreateRef<Scene>();

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

        VXM_INFO("Creat FlatColor Material");
        m_Shader = Shader::Create("assets/shaders/FlatColor.glsl");
        m_Material = CreateRef<Material>(m_Shader);

        VXM_INFO("Creat Texture Material");
        m_TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");
        m_TextureMaterial = CreateRef<Material>(m_TextureShader);

        m_Texture = Texture2D::Create("assets/textures/texture_checker.png");
    }

    void EditorLayer::OnUpdate(TimeStep timeStep) {
        VXM_PROFILE_FUNCTION();

        // Resize
        m_ActiveScene->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
                m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
                (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_Camera.SetSize(m_ViewportSize.x, m_ViewportSize.y);
//            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

        if(m_CameraEnable && !Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            m_Camera.SetEnable(m_CameraEnable = false);
        }
        else if(!m_CameraEnable && Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && m_ViewportHovered)
        {
            m_Camera.SetEnable(m_CameraEnable = true);
        }

        m_Camera.OnUpdate(timeStep);

        {
            VXM_PROFILE_SCOPE("Rendering Preparation.");
            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
            RenderCommand::Clear();
        }

        m_Texture->Bind();

        {
            VXM_PROFILE_SCOPE("Rendering Scene");
//            Renderer::BeginScene(m_Camera.GetCamera());

            m_ActiveScene->OnUpdate(timeStep);

//            Renderer::Submit(m_TextureShader, m_SquareVertexArray);
//            Renderer::Submit(m_Shader, m_VertexArray, Math::TRS(m_ModelPos, glm::quat(glm::radians(m_ModelRot)), m_ModelScale));

//            Renderer::EndScene();
            m_Framebuffer->Unbind();
        }
    }

    void EditorLayer::OnImGuiRender() {
        VXM_PROFILE_FUNCTION();
        RenderDockspace();

        m_SceneHierarchyPanel.OnImGuiRender();

        DrawImGuiViewport();
    }

    void EditorLayer::RenderDockspace()
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
        ImGuiStyle& style = ImGui::GetStyle();
        float minSizeWidthX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
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
        style.WindowMinSize.x = minSizeWidthX;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                RenderMenuBar();
                ImGui::EndMenu();
            }

            // Disabling the following option menu but keeping it for reference.
//                if (ImGui::BeginMenu("Options"))
//                {
//                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
//                    // which we can't undo at the moment without finer window depth/z control.
//                    ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
//                    ImGui::MenuItem("Padding", NULL, &opt_padding);
//                    ImGui::Separator();
//
//                    if (ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
//                    if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
//                    if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
//                    if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
//                    if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
//                    ImGui::Separator();
//
//                    if (ImGui::MenuItem("Close Dockspace", NULL, false))
//                        dockspaceOpen = false;
//                    ImGui::EndMenu();
//                }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& event) {
        VXM_PROFILE_FUNCTION();
        m_Camera.OnEvent(event);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }

    void EditorLayer::OnAttach()
    {
        VXM_PROFILE_FUNCTION();
        FramebufferSpecification specification(1280, 720);
        m_ViewportSize = {specification.Width, specification.Height};
        m_Framebuffer = Framebuffer::Create(specification);
        m_ActiveScene->SetViewportSize(specification.Width, specification.Height);

        m_CubeEntity = m_ActiveScene->CreateEntity("Cube");
        auto& cubeTransform = m_CubeEntity.GetComponent<TransformComponent>();
        cubeTransform.SetPosition(m_ModelPos);
        cubeTransform.SetRotation(glm::quat(glm::radians(m_ModelRot)));
        cubeTransform.SetScale(m_ModelScale);
        m_CubeEntity.AddComponent<MeshComponent>(m_Material, m_VertexArray);

        m_TextureEntity = m_ActiveScene->CreateEntity("Texture Entity");
        m_TextureEntity.AddComponent<MeshComponent>(m_TextureMaterial, m_SquareVertexArray);

        m_ActiveCamera = m_ActiveScene->CreateEntity("Camera");
        m_ActiveCamera.AddComponent<CameraComponent>();

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        auto& imguiLayer = *Application::Get().GetImGuiLayer();
        imguiLayer.AddFont("assets/fonts/OpenSans/OpenSans-Regular.ttf", 18.0f, FontType::Regular, true);
        imguiLayer.AddFont("assets/fonts/OpenSans/OpenSans-Bold.ttf", 18.0f, FontType::Bold);
    }

    void EditorLayer::OnDetach()
    {
        VXM_PROFILE_FUNCTION();
    }

    void EditorLayer::DrawImGuiViewport()
    {
        VXM_PROFILE_FUNCTION();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = glm::uvec2(static_cast<uint32_t>(viewportPanelSize.x), static_cast<uint32_t>(viewportPanelSize.y));

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::RenderMenuBar()
    {
        if(ImGui::MenuItem("New", "Ctr+N"))
        {
            CreateNewScene();
        }

        if(ImGui::MenuItem("Open...", "Ctr+O"))
        {
            OpenScene();
        }

        if(ImGui::MenuItem("Save as...", "Ctr+Shift+S"))
        {
            SaveSceneAs();
        }

        if(ImGui::MenuItem("Save", "Ctr+S"))
        {
            SaveScene();
        }

        if(ImGui::MenuItem("Exit", "Alt+F4"))
        {
            Application::Get().Close();
        }
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if(e.GetRepeatCount() > 0) return false;

        bool control = Input::IsKeyPressed(KeyCode::KEY_LEFT_CONTROL) || Input::IsKeyPressed(KeyCode::KEY_RIGHT_CONTROL);
        if(!control) return false;

        switch (e.GetKeyCode()) {
            case KeyCode::KEY_S:
            {
                bool shift = Input::IsKeyPressed(KeyCode::KEY_LEFT_SHIFT) || Input::IsKeyPressed(KeyCode::KEY_RIGHT_SHIFT);
                if(shift)
                {
                    SaveSceneAs();
                }
                else
                {
                    SaveScene();
                }

                break;
            }
            case KeyCode::KEY_N:
            {
                CreateNewScene();
                break;
            }
            case KeyCode::KEY_O:
            {
                OpenScene();
                break;
            }
            default:
            {
                break;
            }
        }
        return false;
    }

    void EditorLayer::SaveSceneAs()
    {
        VXM_CORE_TRACE("Save Scene As");
        std::string file = FileDialogs::SaveFile({"Voxymore Scene (*.vxm)", "*.vxm"});
        if(!file.empty())
        {
            if(!file.ends_with(".vxm")) file.append(".vxm");
            m_FilePath = file;
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_FilePath);
        }
    }

    void EditorLayer::CreateNewScene()
    {
        VXM_CORE_TRACE("Create New Scene");
        m_FilePath = "";
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::SaveScene()
    {
        if(!m_FilePath.empty())
        {
            VXM_CORE_TRACE("Save Scene");
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_FilePath);
        }
        else
        {
            SaveSceneAs();
        }
    }

    void EditorLayer::OpenScene()
    {
        VXM_CORE_TRACE("Open Scene");
        std::string file = FileDialogs::OpenFile({"Voxymore Scene (*.vxm)", "*.vxm"});
        if(!file.empty())
        {
            CreateNewScene();
            m_FilePath = file;

            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(m_FilePath);
        }
    }
} // Voxymore
// Editor