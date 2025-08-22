#include <iostream>
#include <print>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_TITLE "GameEngine Luau"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

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
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::println("Failed to load GLAD");
		return 2;
	}

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.8f, 1.0f, 1.0f); // Test!! 123 123

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	

	return 0;
}
