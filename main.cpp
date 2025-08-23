/* STD DEPENDENCIES */
#include <iostream>
#include <print>
#include <memory>

/* EXTERN DEPENDENCIES */
#include <glad/glad.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* MESH */
#include <Mesh/IMesh.h>
#include <Mesh/Mesh3D.h>

/* SHADER */
#include <Shader/Shader.h>

/* RENDERER */
#include <Render/MeshRenderer.h>

/* CAMERA */
#include <Camera/ICamera.h>
#include <Camera/Camera3D.h>

/* GLB DESERIALIZER */
#include <MeshDeserializer/GlbDeserializer.h>

/* RESOURCE MANAGER */
#include <ResourceManager/Managers/MeshManager.h>
#include <ResourceManager/ResourceManager.h>

/* CONSTANTS */
#define WINDOW_TITLE "GameEngine Luau"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

/* GLSL SHADERS */

std::string vertexSrc = R"(#version 460

layout(location=0) in vec3 vertex;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 vertexColor;
layout(location=3) in vec2 textureCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 aTextureCoords;
out vec3 aNormal;
out vec3 aVertexColor;

void main(){
	gl_Position = projection * view * model * vec4(vertex, 1.0);
	aTextureCoords = textureCoords;
	aNormal = normalize(normal);
	aVertexColor = vertexColor;
}

)";

std::string fragmentSrc = R"(#version 460

uniform sampler2D texture;

in vec2 aTextureCoords;
in vec3 aNormal;
in vec3 aVertexColor;
out vec4 OutputColor;

void main(){
	OutputColor = vec4(aNormal, 1.0);
}

)";


/* GLOBALS */
bool showUi = true;
bool fullscreen = false;
bool mouseLocked = false;
bool wireframeMode = false;

int windowPosX = 0, windowPosY = 0;
int windowWidth = WINDOW_WIDTH, windowHeight = WINDOW_HEIGHT;

ResourceManager& resourceManager = ResourceManager::getInstance();

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
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		mouseLocked = !mouseLocked;
		glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	const auto mainShader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
	const std::vector<Mesh3D::Vertex> planeVertices{
		Mesh3D::Vertex{ glm::vec3{-0.5f, 0.0f, 0.5f}, glm::vec3{1.0f, 1.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 1.0f},}, // Left Back
		Mesh3D::Vertex{ glm::vec3{-0.5f, 0.0f, -0.5f}, glm::vec3{1.0f, 1.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 0.0f}, }, // Left Front
		Mesh3D::Vertex{ glm::vec3{0.5f, 0.0f, -0.5f}, glm::vec3{1.0f, 1.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, }, // Right Front
		Mesh3D::Vertex{ glm::vec3{0.5f, 0.0f, 0.5f}, glm::vec3{1.0f, 1.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, }, // Right Back
	};
	const std::vector<unsigned int> planeIndices{
		0, 1, 2,
		0, 3, 2
	};
	const auto object = resourceManager.meshManager.loadMeshFromFile("./resources/pumpkin.glb");

	const auto plane = std::make_unique<Mesh3D>(planeVertices, planeIndices);
	const auto currentCamera = std::make_unique<Camera3D>();
	currentCamera->position = glm::vec3{ 0.0f, 2.0f, 2.0f };

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	double oldMouseX = 0.0f, oldMouseY = 0.0f;
	glfwGetCursorPos(window, &oldMouseX, &oldMouseY);

	double glfwTime = 0;
	double lastGlfwTime = 0;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

		if (wireframeMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glfwTime = glfwGetTime();
		double deltaTime = glfwTime - lastGlfwTime;
		lastGlfwTime = glfwTime;

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);

		float fWidth = static_cast<float>(width), fHeight = static_cast<float>(height);
		float aspect = fWidth / fHeight;

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		float fMouseX = static_cast<float>(mouseX), fMouseY = static_cast<float>(mouseY);

		float oldFMouseX = static_cast<float>(oldMouseX);
		float oldFMouseY = static_cast<float>(oldMouseY);

		float deltaX = fMouseX - oldFMouseX, deltaY = fMouseY - oldFMouseY;

		
		oldMouseX = mouseX;
		oldMouseY = mouseY;

		//std::println("Mouse Delta: ({}, {})", deltaX, deltaY);
		currentCamera->aspect = aspect > 0.0f ? aspect : currentCamera->aspect;
		currentCamera->rotation += glm::vec3{ deltaY, deltaX, 0.0f };
		currentCamera->rotation.x = glm::clamp(currentCamera->rotation.x, -89.0f, 89.0f);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			float x = glm::sin(glm::radians(currentCamera->rotation.y));
			float z = glm::cos(glm::radians(currentCamera->rotation.y));
			currentCamera->position += glm::vec3{x * 10.0f * deltaTime,0.0f,-z * 10.0f * deltaTime };
		} 
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			float x = glm::sin(glm::radians(currentCamera->rotation.y));
			float z = glm::cos(glm::radians(currentCamera->rotation.y));
			currentCamera->position += glm::vec3{ -x * 10.0f * deltaTime,0.0f,z * 10.0f * deltaTime };
		}  
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			float x = glm::sin(glm::radians(currentCamera->rotation.y-90.0f));
			float z = glm::cos(glm::radians(currentCamera->rotation.y-90.0f));
			currentCamera->position += glm::vec3{ x * 10.0f * deltaTime,0.0f,-z * 10.0f * deltaTime };
		} 
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			float x = glm::sin(glm::radians(currentCamera->rotation.y + 90.0f));
			float z = glm::cos(glm::radians(currentCamera->rotation.y + 90.0f));
			currentCamera->position += glm::vec3{ x * 10.0f * deltaTime,0.0f,-z * 10.0f * deltaTime };
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			currentCamera->position += glm::vec3{0.0f, 10.0f * deltaTime, 0.0f};
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			currentCamera->position += glm::vec3{ 0.0f, -10.0f * deltaTime, 0.0f };
		}

		mainShader->use();
		mainShader->setMat4("projection", currentCamera->getProjectionMatrix());
		mainShader->setMat4("view", currentCamera->getViewMatrix());
		mainShader->setMat4("model", glm::identity<glm::mat4>());
		MeshRenderer::draw(*object);

		if (showUi) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("Developer menu");
			ImGui::Text("## GameEngine ##");
			if (ImGui::Button("Wireframe Mode")) {
				wireframeMode = !wireframeMode;
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/* DEINIT */
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
