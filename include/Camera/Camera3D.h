#pragma once
#include <Camera/ICamera.h>
#include <glm/glm.hpp>

class Camera3D: public ICamera {
public:
	float fieldOfView = 80.0f;
	float aspect = 1.0f;
	float nearPlaneZ = 0.1f;
	float farPlaneZ = 1000.0f;
	glm::vec3 position;
	glm::vec3 rotation;

	Camera3D() = default;
	~Camera3D() = default;

	glm::mat4 getViewMatrix() const override;
	glm::mat4 getProjectionMatrix() const override;
};