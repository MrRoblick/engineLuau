cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("engineLuau")

set(EXTERN "${CMAKE_SOURCE_DIR}/extern")
set(GLAD "${EXTERN}/glad")
set(GLFW "${EXTERN}/glfw")
set(GLM "${EXTERN}/glm")

add_executable(
	GameEngineLuau
	"main.cpp"
	"${GLAD}/src/glad.c"
)
target_include_directories(
	GameEngineLuau PRIVATE
	"${GLAD}/include"
	"${GLFW}/include"
	"${GLM}/include"
)
target_link_libraries(
	GameEngineLuau PRIVATE
	"${GLFW}/lib/glfw3.lib"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngineLuau PROPERTY CXX_STANDARD 23)
endif()