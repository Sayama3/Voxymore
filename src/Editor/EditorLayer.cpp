//
// Created by ianpo on 21/08/2023.
//

#include "Voxymore/Editor/EditorLayer.hpp"
#include "Voxymore/Utils/Platform.hpp"
#include <ImGuizmo.h>


namespace Voxymore::Editor {
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_EditorCamera(60.0f, 1280.0f/ 720.0f, 0.1f, 1000.0f)
    {
        VXM_PROFILE_FUNCTION();
        Application::Get().GetWindow().SetCursorState(CursorState::None);
        const Window& window = Application::Get().GetWindow();

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
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

//        if(m_CameraEnable && !Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
//        {
//            m_Camera.SetEnable(m_CameraEnable = false);
//        }
//        else if(!m_CameraEnable && Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && m_ViewportHovered)
//        {
//            m_Camera.SetEnable(m_CameraEnable = true);
//        }

        m_EditorCamera.OnUpdate(timeStep);

        {
            VXM_PROFILE_SCOPE("Rendering Preparation.");
            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
            RenderCommand::Clear();
            m_Framebuffer->ClearColorAttachment(1, -1);
            m_Texture->Bind();
        }

        {
            VXM_PROFILE_SCOPE("Rendering Scene");

            m_ActiveScene->OnUpdateEditor(timeStep, m_EditorCamera);
//            m_ActiveScene->OnUpdateRuntime(timeStep);

            {
                auto [fMouseX, fMouseY] = ImGui::GetMousePos();
                fMouseX -= m_ViewportBounds[0].x;
                fMouseY -= m_ViewportBounds[0].y;

                auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
                auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

                //Reverse Y because the texture is reversed.
                fMouseY = viewportHeight - fMouseY;

                int mouseX = fMouseX;
                int mouseY = fMouseY;

                if(mouseX > 0 && mouseX < viewportWidth
                && mouseY > 0 && mouseY < viewportHeight)
                {
                    int value = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
                    VXM_CORE_TRACE("Pixel Data:; {0}", value);
                    if(value < 0)
                    {
                        m_HoveredEntity = Entity();
                    }
                    else{
                        auto entityId = static_cast<uint32_t>(value);
                        m_HoveredEntity = Entity(static_cast<entt::entity>(entityId), m_ActiveScene.get());
                    }
                    //TODO: pick hovered entity.
                }
            }

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
        m_EditorCamera.OnEvent(event);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMousePressed));
    }

    void EditorLayer::OnAttach()
    {
        VXM_PROFILE_FUNCTION();
        FramebufferSpecification specification(1280, 720);
        specification.Attachements = {FramebufferTextureFormat::Color, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth};
        m_ViewportSize = {specification.Width, specification.Height};
        m_Framebuffer = Framebuffer::Create(specification);
        m_ActiveScene->SetViewportSize(specification.Width, specification.Height);

        m_CubeEntity = m_ActiveScene->CreateEntity("Cube");
        auto& cubeTransform = m_CubeEntity.GetComponent<TransformComponent>();
        cubeTransform.SetPosition({0, 0, -2});
        cubeTransform.SetRotation(glm::quat(glm::radians(glm::vec3{0, 0, 0})));
        cubeTransform.SetScale({1, 1, 1});
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
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);
        m_EditorCamera.SetViewportFocused(m_ViewportFocused);
        m_EditorCamera.SetViewportHovered(m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = glm::uvec2(static_cast<uint32_t>(viewportPanelSize.x), static_cast<uint32_t>(viewportPanelSize.y));

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        DrawGizmos();

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

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

        switch (e.GetKeyCode()) {
            case Key::S:
            {
                if(control)
                {
                    if (shift) {
                        SaveSceneAs();
                    } else {
                        SaveScene();
                    }
                }

                break;
            }
            case Key::N:
            {
                if(control) CreateNewScene();
                break;
            }
            case Key::O:
            {
                if(control) OpenScene();
                break;
            }
            case Key::Q:
            {
                if(!(control || shift || alt)) m_GizmoOperation = GizmoOperation::NONE;
                break;
            }
            case Key::W:
            {
                if(!(control || shift || alt)) m_GizmoOperation = GizmoOperation::TRANSLATE;
                break;
            }
            case Key::E:
            {
                if(!(control || shift || alt)) m_GizmoOperation = GizmoOperation::ROTATE;
                break;
            }
            case Key::R:
            {
                if(!(control || shift || alt)) m_GizmoOperation = GizmoOperation::SCALE;
                break;
            }
            case Key::T:
            {
                if(!(control || shift || alt)) m_GizmoOperation = GizmoOperation::UNIVERSAL;
                break;
            }
            case Key::Z:
            {
                if(!(control || shift || alt)) m_GizmoMode = m_GizmoMode == GizmoMode::WORLD ? GizmoMode::LOCAL : GizmoMode::WORLD;
                break;
            }
            case Key::F:
            {
                if(!(control || shift || alt))
                {
                    auto selected = m_SceneHierarchyPanel.GetSelectedEntity();
                    if (selected) {
                        auto selectedTc = selected.GetComponent<TransformComponent>();
                        m_EditorCamera.SetFocusTarget(selectedTc.GetPosition());
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
        return false;
    }

    bool EditorLayer::OnMousePressed(MouseButtonPressedEvent& e)
    {
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

        switch (e.GetMouseButton())
        {
            case Mouse::Left:
            {
                if (m_ViewportHovered)
                {
                    if(!m_SceneHierarchyPanel.GetSelectedEntity().IsValid() || m_GizmoOperation == GizmoOperation::NONE) m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
                    else if (!ImGuizmo::IsOver() && !control && !shift && !alt) m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
                }
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

    void EditorLayer::DrawGizmos()
    {
        DrawGizmosWindow();
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if(selectedEntity)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            // Runtime Camera
//            Entity cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
//            const auto& cameraComponent = cameraEntity.GetComponent<CameraComponent>();
//            const auto& transformComponent = cameraEntity.GetComponent<TransformComponent>();
//
//            const glm::mat4 projectionMatrix = cameraComponent.Camera.GetProjectionMatrix();
//            const glm::mat4 viewMatrix = glm::inverse(transformComponent.GetTransform());

            // Editor Camera
            const glm::mat4 projectionMatrix = m_EditorCamera.GetProjectionMatrix();
            const glm::mat4& viewMatrix = m_EditorCamera.GetViewMatrix();

            // Selected Entity
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 modelMatrix = tc.GetTransform();

            bool isSnapping = (Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl)) && m_GizmoOperation != GizmoOperation::UNIVERSAL;

            float snapValue= m_GizmoOperation == GizmoOperation::TRANSLATE
                             ? m_GizmoTranslationSnapValue
                             : m_GizmoOperation == GizmoOperation::ROTATE
                               ? m_GizmoRotationSnapValue
                               : m_GizmoScaleSnapValue;

            float snapValues[3] = {snapValue, snapValue, snapValue};
            bool manipulate = ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), static_cast<ImGuizmo::OPERATION>(m_GizmoOperation), static_cast<ImGuizmo::MODE>(m_GizmoMode), glm::value_ptr(modelMatrix), nullptr, isSnapping ? snapValues : nullptr);
            m_EditorCamera.EnableMovement(!manipulate);
            if(ImGuizmo::IsUsing())
            {
                glm::vec3 scale;
                glm::vec3 position;
                glm::quat rotation;

                Math::DecomposeTransform(modelMatrix, position, rotation, scale);

                tc.SetPosition(position);
                tc.SetRotation(rotation);
                tc.SetScale(scale);
            }
        }
    }

    void EditorLayer::DrawGizmosWindow()
    {
        ImGui::Begin("Gizmo Params");

        if (ImGui::RadioButton("Select", m_GizmoOperation == GizmoOperation::NONE))
            m_GizmoOperation = GizmoOperation::NONE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Translate", m_GizmoOperation == GizmoOperation::TRANSLATE))
            m_GizmoOperation = GizmoOperation::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", m_GizmoOperation == GizmoOperation::ROTATE))
            m_GizmoOperation = GizmoOperation::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", m_GizmoOperation == GizmoOperation::SCALE))
            m_GizmoOperation = GizmoOperation::SCALE;
        ImGui::SameLine();
        if (ImGui::RadioButton("All", m_GizmoOperation == GizmoOperation::UNIVERSAL))
            m_GizmoOperation = GizmoOperation::UNIVERSAL;

        if (m_GizmoOperation != GizmoOperation::SCALE)
        {
            if (ImGui::RadioButton("Local", m_GizmoMode == GizmoMode::LOCAL))
                m_GizmoMode = GizmoMode::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", m_GizmoMode == GizmoMode::WORLD))
                m_GizmoMode = GizmoMode::WORLD;
        }
        ImGui::End();
    }
} // Voxymore
// Editor