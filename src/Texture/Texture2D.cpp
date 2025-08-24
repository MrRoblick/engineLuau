#include <Texture/Texture2D.h>
#include <string>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stdexcept>
Texture2D::Texture2D(const std::string& filename) {
	if (filename.empty()) {
		throw std::runtime_error("Texture2D: filename is empty");
		return;
	}
	auto image = stbi_load(filename.c_str(), reinterpret_cast<int*>(&m_size.x), reinterpret_cast<int*>(&m_size.y), nullptr, 4);
	
	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image);
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