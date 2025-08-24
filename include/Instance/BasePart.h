#pragma once
#include <Instance/Instance.h>
#include <glm/glm.hpp>

class BasePart : public Instance {
public:
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 orientation{0.0f, 0.0f, 0.0f};
	glm::vec3 size{ 4.0f, 1.0f, 2.0f };
	glm::u8vec3 color{ 163, 162, 165 };
	float transparency = 0.0f;

	bool castShadow = true;
	bool canCollide = true;
	bool anchored = true;

	BasePart() { name = "BasePart"; };
	~BasePart() = default;
};