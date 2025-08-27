#pragma once

#include <glm/glm.hpp>

class ICamera {
public:
	~ICamera() = default;
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::mat4 getRotationMatrix() const = 0;
	virtual glm::mat4 getProjectionMatrix() const = 0;
};