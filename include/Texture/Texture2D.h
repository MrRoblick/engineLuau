#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Texture2D {
public:
	Texture2D(const std::string& filename);
	~Texture2D();

	void bind(unsigned int slot = 0) const;
	glm::uvec2 getSize() const;
private:
	GLuint m_id = 0;
	glm::uvec2 m_size{ 0, 0 };
};