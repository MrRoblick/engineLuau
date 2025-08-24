#include <string>
#include <glad/glad.h>
#include <stdexcept>
#include <Texture/Texture2D.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <memory>

Texture2D::Texture2D(const std::string& filename) {
	if (filename.empty()) {
		throw std::runtime_error("Texture2D: filename is empty");
		return;
	}

	int width = 0, height = 0;
	auto image = stbi_load(filename.c_str(), &width, &height, nullptr, 4);
	if (!image) {
		throw std::runtime_error("Texture2D: failed to load image");
	}
	m_size = glm::uvec2(width, height);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);
}
void Texture2D::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

glm::uvec2 Texture2D::getSize() const {
	return m_size;
}

Texture2D::~Texture2D() {
	if (m_id != 0) {
		glDeleteTextures(1, &m_id);
	}
}
