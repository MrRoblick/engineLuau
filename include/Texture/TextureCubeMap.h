#pragma once
#include <Texture/ITexture.h>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

class TextureCubeMap: public ITexture {
public:
	TextureCubeMap(
		const std::string& frontFilename,
		const std::string& backFilename,
		const std::string& leftFilename,
		const std::string& rightFilename,
		const std::string& topFilename,
		const std::string& bottomFilename
	);
	~TextureCubeMap();

	void bind(unsigned int slot = 0) const override;
private:
	GLuint m_id = 0;
};