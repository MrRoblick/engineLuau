#include <Camera/Camera3D.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>


glm::mat4 Camera3D::getViewMatrix() const {
	glm::mat4 mat = glm::identity<glm::mat4>();
	mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
	mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
	mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });

	mat = glm::translate(mat, -position);
	return mat;
}
glm::mat4 Camera3D::getProjectionMatrix() const {
	return glm::perspective(
		glm::radians(fieldOfView),
		aspect,
		nearPlaneZ,
		farPlaneZ
	);
}