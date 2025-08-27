#include <Texture/TextureCubeMap.h>
#include <string>
#include <glad/glad.h>
#include <stb/stb_image.h>


TextureCubeMap::TextureCubeMap(
	const std::string& frontFilename,
	const std::string& backFilename,
	const std::string& leftFilename,
	const std::string& rightFilename,
	const std::string& topFilename,
	const std::string& bottomFilename
) {
	const char* const faces[6] = {
		rightFilename.c_str(),
		leftFilename.c_str(),
		topFilename.c_str(),
		bottomFilename.c_str(),
		backFilename.c_str(),
		frontFilename.c_str(),
	};
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	for (size_t i = 0; i < 6; i++) {
		const auto texFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

		int width, height;
		const auto data = stbi_load(faces[i], &width, &height, nullptr, 3);
		if (!data) continue;
		glTexImage2D(texFace, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCubeMap::bind(unsigned int slot) const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}
TextureCubeMap::~TextureCubeMap() {
	if (!m_id) {
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}