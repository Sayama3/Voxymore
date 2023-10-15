#define NEWLINE "\n"

#include "Voxymore/Voxymore.hpp"
#include "Voxymore/Core/EntryPoint.hpp"
#include "Voxymore/Editor/EditorLayer.hpp"

namespace Voxymore::Editor {

class Voxengine : public Voxymore::Core::Application {
    public:
        Voxengine(const ApplicationParameters& parameters) : Voxymore::Core::Application(parameters) {
            PushLayer(new EditorLayer());
        }

        ~Voxengine() {}
    };
}

Voxymore::Core::Application* Voxymore::Core::CreateApplication(int argc, char** argv) {
    VXM_PROFILE_FUNCTION();

//#if VXM_DEBUG
//    VXM_TRACE("Create Application with argument :");
//    for (int i = 0; i < argc; ++i) {
//        VXM_TRACE("  {0}: {1}",i, argv[i]);
//    }
//#endif

    std::filesystem::path exePath = argv[0];

    ApplicationParameters parameters;
    parameters.name = "Voxengine";
    parameters.argc = argc;
    parameters.argv = argv;

    return new Voxymore::Editor::Voxengine(parameters);
}