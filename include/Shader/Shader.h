#pragma once

#include <glad/glad.h>
#include <string>

class Shader {
private:
	GLuint m_programId = 0;
	GLuint m_vertexShaderId = 0;
	GLuint m_fragmentShaderId = 0;
	static GLuint compileShader(const std::string& source, GLenum shaderType);
public:
	explicit Shader(const std::string& vertex, const std::string& fragment);
	~Shader();

	void use() const;
};