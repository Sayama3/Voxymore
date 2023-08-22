#define NEWLINE "\n"

#include "Voxymore/Voxymore.hpp"
#include "Voxymore/Core/EntryPoint.hpp"
#include "Voxymore/Editor/EditorLayer.hpp"

namespace Voxymore::Editor {

class Voxengine : public Voxymore::Core::Application {
    public:
        Voxengine() : Voxymore::Core::Application("Voxengine") {
            PushLayer(new EditorLayer());
        }

        ~Voxengine() {}
    };
}

Voxymore::Core::Application* Voxymore::Core::CreateApplication(int argc, char** argv) {
    VXM_PROFILE_FUNCTION();

#if VXM_DEBUG
    VXM_TRACE("Create Application with argument :");
    for (int i = 0; i < argc; ++i) {
        VXM_TRACE("  i: {0}", argv[i]);
    }
#endif

    return new Voxymore::Editor::Voxengine();
}