#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

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
	
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;

	void setInt(const std::string& name, const int value) const;

	void setFloat(const std::string& name, float value) const;
	void setFloat2(const std::string& name, float x, float y) const;
	void setFloat3(const std::string& name, float x, float y, float z) const;

	void setFloat2(const std::string& name, glm::vec2 vec) const;
	void setFloat3(const std::string& name, glm::vec3 vec) const;
};