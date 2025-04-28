cmake_minimum_required(VERSION 3.16)
project(QtVulkanApp LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(QtVulkanApp
    Renderer.cpp Renderer.h
    MainWindow.cpp MainWindow.h
    main.cpp
    VulkanWindow.h VulkanWindow.cpp
    TriangleSurface.h TriangleSurface.cpp
    VisualObject.h VisualObject.cpp
    Vertex.h Vertex.cpp
    Camera.h Camera.cpp
    Triangle.h Triangle.cpp
    Input.h
    WorldAxis.h WorldAxis.cpp
    Utilities.h
    stb_image.h
    stb_image.cpp
    HeightMap.h HeightMap.cpp
    ObjMesh.h ObjMesh.cpp
)
# Define the shader files
set(SHADER_FILES
    color.frag
    color.vert
    texture.frag
    texture.vert
)

# Add the shader files to the project
add_custom_target(Shaders
    SOURCES ${SHADER_FILES}
)

set_target_properties(QtVulkanApp PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(QtVulkanApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Resources:
set_source_files_properties("color_frag.spv"
    PROPERTIES QT_RESOURCE_ALIAS "color_frag.spv"
)

set_source_files_properties("color_vert.spv"
    PROPERTIES QT_RESOURCE_ALIAS "color_vert.spv"
)

set_source_files_properties("texture_frag.spv"
    PROPERTIES QT_RESOURCE_ALIAS "texture_frag.spv"
)

set_source_files_properties("texture_vert.spv"
    PROPERTIES QT_RESOURCE_ALIAS "texture_vert.spv"
)

set(QtVulkanApp_resource_files
    "color_frag.spv"
    "color_vert.spv"
    "texture_frag.spv"
    "texture_vert.spv"
)

qt_add_resources(QtVulkanApp "QtVulkanApp"
    PREFIX
        "/"
    FILES
        ${QtVulkanApp_resource_files}
)

install(TARGETS QtVulkanApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtVulkanApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# auto-compilation of the shaders:
add_custom_target(
    PreBuildCommandCF ALL
    COMMAND glslc color.frag -o color_frag.spv
#   This version includes the debug info in the compiled shader - good for RenderDoc
#   COMMAND glslangValidator -g -V -o color_frag.spv color.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling color fragment shader"
)
add_custom_target(
    PreBuildCommandCV ALL
    COMMAND glslc color.vert -o color_vert.spv
#   COMMAND glslangValidator -g -V -o color_vert.spv color.vert
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling color vertex shader"
)
add_custom_target(
    PreBuildCommandTF ALL
    COMMAND glslc texture.frag -o texture_frag.spv
#   This version includes the debug info in the compiled shader - good for RenderDoc
#   COMMAND glslangValidator -g -V -o texture_frag.spv texture.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling texture fragment shader"
)
add_custom_target(
    PreBuildCommandTV ALL
    COMMAND glslc texture.vert -o texture_vert.spv
#   COMMAND glslangValidator -g -V -o texture_vert.spv texture.vert
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling texture vertex shader"
)

add_dependencies(QtVulkanApp PreBuildCommandCF)
add_dependencies(QtVulkanApp PreBuildCommandCV)
add_dependencies(QtVulkanApp PreBuildCommandTF)
add_dependencies(QtVulkanApp PreBuildCommandTV)


