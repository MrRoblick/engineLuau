#include <Shader/Shader.h>
#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint Shader::compileShader(const std::string& source, GLenum shaderType) {
	GLuint id = glCreateShader(shaderType);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	GLint status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint logLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

		auto log = std::vector<GLchar>();
		log.resize(logLength + 1);

		glGetShaderInfoLog(id, logLength, nullptr, log.data());

		const auto stringLog = "Failed to compile shader: " + std::string(log.begin(), log.end());
		std::cout << stringLog << std::endl;
		throw std::runtime_error(stringLog);
	}
	return id;
}

Shader::Shader(const std::string& vertex, const std::string& fragment) {
	m_vertexShaderId = Shader::compileShader(vertex, GL_VERTEX_SHADER);
	m_fragmentShaderId = Shader::compileShader(fragment, GL_FRAGMENT_SHADER);

	m_programId = glCreateProgram();
	glAttachShader(m_programId, m_vertexShaderId);
	glAttachShader(m_programId, m_fragmentShaderId);
	glLinkProgram(m_programId);

	GLint status = 0;
	glGetProgramiv(m_programId, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint logLength = 0;
		glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &logLength);

		auto log = std::vector<GLchar>();
		log.resize(logLength + 1);

		glGetProgramInfoLog(m_programId, logLength, nullptr, log.data() );

		std::cout << logLength << std::endl;
		const auto stringLog = "Failed to link program: " + std::string(log.begin(), log.end());
		std::cout << stringLog << std::endl;
		throw std::runtime_error(stringLog);
	}
}

Shader::~Shader() {
	if (m_programId) glDeleteProgram(m_programId);

	if (m_vertexShaderId) glDeleteShader(m_vertexShaderId);
	if (m_fragmentShaderId) glDeleteShader(m_fragmentShaderId);

	m_programId = 0;
	m_vertexShaderId = 0;
	m_fragmentShaderId = 0;
}

void Shader::use() const {
	glUseProgram(m_programId);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glProgramUniformMatrix4fv(m_programId, glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
	glProgramUniformMatrix3fv(m_programId, glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setFloat(const std::string& name, float value) const{
	glProgramUniform1f(m_programId, glGetUniformLocation(m_programId, name.c_str()), value);
}
void Shader::setFloat2(const std::string& name, float x, float y) const{
	glProgramUniform2f(m_programId, glGetUniformLocation(m_programId, name.c_str()), x, y);
}
void Shader::setFloat3(const std::string& name, float x, float y, float z) const{
	glProgramUniform3f(m_programId, glGetUniformLocation(m_programId, name.c_str()), x, y, z);
}
void Shader::setFloat2(const std::string& name, glm::vec2 vec) const{
	glProgramUniform2fv(m_programId, glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(vec));
}
void Shader::setFloat3(const std::string& name, glm::vec3 vec) const {
	glProgramUniform3fv(m_programId, glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(vec));
}