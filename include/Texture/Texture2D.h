#pragma once
#include <Texture/ITexture.h>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <memory>

class Texture2D: public ITexture {
public:
	Texture2D(const std::string& filename);
	~Texture2D();

	void bind(unsigned int slot = 0) const override;
	glm::uvec2 getSize() const override;
private:
	GLuint m_id = 0;
	glm::uvec2 m_size{ 0, 0 };
};