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

	explicit Camera3D() = default;
	explicit Camera3D(glm::vec3 position, glm::vec3 rotation, float fieldOfView, float nearPlaneZ, float farPlaneZ, float aspect)
		: position(position), rotation(rotation), fieldOfView(fieldOfView), nearPlaneZ(nearPlaneZ), farPlaneZ(farPlaneZ), aspect(aspect) {};
	~Camera3D() = default;

	glm::mat4 getViewMatrix() const override;
	glm::mat4 getRotationMatrix() const override;
	glm::mat4 getProjectionMatrix() const override;
};