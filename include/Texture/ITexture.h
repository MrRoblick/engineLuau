#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

class ITexture {
public:
	virtual ~ITexture() = default;
	virtual void bind(unsigned int slot = 0) const = 0;
	//virtual glm::uvec2 getSize() const = 0;
};