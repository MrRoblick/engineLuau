/* STD DEPENDENCIES */
#include <iostream>
#include <print>

/* EXTERN DEPENDENCIES */
#include <glad/glad.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

/* MESH */
#include <Mesh/IMesh.h>
#include <Mesh/Mesh3D.h>

/* SHADER */
#include <Shader/Shader.h>

/* RENDERER */
#include <Render/MeshRenderer.h>

/* CONSTANTS */
#define WINDOW_TITLE "GameEngine Luau"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

/* GLSL SHADERS */

std::string vertexSrc = R"(#define 460

layout(location=0) in vec3 vertex;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 textureCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 aTextureCoords;

void main(){
	gl_Position = projection * view * model * vec4(vertex, 1.0);
	aTextureCoords = textureCoords;
}

)";

std::string fragmentSrc = R"(#define 460

uniform sampler2D texture;

in vec2 aTextureCoords;
out vec4 outputColor;

void main(){
	outputColor = vec4(1.0);
}

)";


/* GLOBALS */
bool showUi = true;
bool fullscreen = false;
int windowPosX = 0, windowPosY = 0;
int windowWidth = WINDOW_WIDTH, windowHeight = WINDOW_HEIGHT;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_INSERT && action == GLFW_PRESS) {
		showUi = !showUi;
	
	}
	else if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
		fullscreen = !fullscreen;

		if (fullscreen) {
			glfwGetWindowPos(window, &windowPosX, &windowPosY);
			glfwGetWindowSize(window, &windowWidth, &windowHeight);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(window, monitor,
				0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else {
			glfwSetWindowMonitor(window, nullptr,
				windowPosX, windowPosY, windowWidth, windowHeight, 0);
		}
	}
}

int main()
{
	if (!glfwInit()) {
		std::println("Failed to load GLFW");
		return 1;
	}
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* const window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	if (!window) {
		std::println("Failed to create window");
		glfwTerminate();
		return 2;
	}
	glfwGetWindowPos(window, &windowPosX, &windowPosY);
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	glfwSetKeyCallback(window, keyCallback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::println("Failed to load GLAD");
		return 3;
	}

	

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

		if (showUi) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("@LLVM Debug Window");
			ImGui::Text("OMG ImGui!!!");
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
