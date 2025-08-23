cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("engineLuau")

set(EXTERN "${CMAKE_SOURCE_DIR}/extern")
set(SRC "${CMAKE_SOURCE_DIR}/src")
set(GLAD "${EXTERN}/glad")
set(GLFW "${EXTERN}/glfw")
set(GLM "${EXTERN}/glm")
set(IMGUI "${EXTERN}/imgui")
set(JSON "${EXTERN}/json")

add_library(glad STATIC "${GLAD}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD}/include")

set(IMGUI_SOURCES
    "${IMGUI}/imgui.cpp"
    "${IMGUI}/imgui_draw.cpp"
    "${IMGUI}/imgui_tables.cpp"
    "${IMGUI}/imgui_widgets.cpp"
    "${IMGUI}/imgui_demo.cpp"
    "${IMGUI}/backends/imgui_impl_glfw.cpp"
    "${IMGUI}/backends/imgui_impl_opengl3.cpp"
)

set(PROJECT_SOURCES
    "${SRC}/Shader/Shader.cpp"
    "${SRC}/Mesh/Mesh3D.cpp"
    "${SRC}/Render/MeshRenderer.cpp"
    "${SRC}/Camera/Camera3D.cpp"
    "${SRC}/MeshDeserializer/GlbDeserializer.cpp"
    "${SRC}/ResourceManager/ResourceManager.cpp"
    "${SRC}/ResourceManager/Managers/MeshManager.cpp"
    "${SRC}/Event/Connection.cpp"
    "${SRC}/Instance/Instance.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI}
    "${IMGUI}/backends"
    "${GLAD}/include"
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui PUBLIC glad)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
  IMPORTED_LOCATION "${GLFW}/lib/glfw3.lib"
  INTERFACE_INCLUDE_DIRECTORIES "${GLFW}/include"
)

find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC glfw3 OpenGL::GL)

add_executable(GameEngineLuau "main.cpp" ${PROJECT_SOURCES})

target_include_directories(GameEngineLuau PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${GLAD}/include"
    "${GLFW}/include"
    "${GLM}/include"
    "${IMGUI}"
    "${IMGUI}/backends"
    "${JSON}/include"
)

target_link_libraries(GameEngineLuau PRIVATE
    imgui
    glad
    glfw3
    OpenGL::GL
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngineLuau PROPERTY CXX_STANDARD 23)
endif()
