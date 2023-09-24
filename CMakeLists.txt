cmake_minimum_required(VERSION 3.24)

# set enable the debugger (when the debug mode is enable?)
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
    endif()
endif()


add_subdirectory(lib/VoxymoreCore)

project(Voxengine VERSION 0.0.1 DESCRIPTION "The Voxymore Game Engine.")

add_executable(Voxengine
        src/Voxengine.cpp
        src/Editor/EditorLayer.cpp
        include/Voxymore/Editor/EditorLayer.hpp
        src/Editor/Panels/SceneHierarchyPanel.cpp
        include/Voxymore/Editor/Panels/SceneHierarchyPanel.hpp
        src/Editor/Panels/PropertyPanel.cpp
        include/Voxymore/Editor/Panels/PropertyPanel.hpp
        src/Editor/Panels/SystemPanel.cpp
        include/Voxymore/Editor/Panels/SystemPanel.hpp
)

set(CMAKE_CXX_STANDARD 20)
target_compile_features(Voxengine PUBLIC cxx_std_20)

include_directories(include)

target_link_libraries(Voxengine PUBLIC Voxymore::Core)
#target_precompile_headers(Voxybox REUSE_FROM Core)
#set_target_properties(Voxybox PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(Voxengine PUBLIC VXM_USE_SIMPLE_FUNCTION)

configure_file(imgui.ini imgui.ini NO_SOURCE_PERMISSIONS COPYONLY)

message(${CMAKE_BINARY_DIR})
file(COPY EditorAssets/ DESTINATION ${CMAKE_BINARY_DIR}/EditorAssets/)
file(COPY EditorShaders/ DESTINATION ${CMAKE_BINARY_DIR}/EditorShaders/)