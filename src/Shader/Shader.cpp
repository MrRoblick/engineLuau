#include <Shader/Shader.h>
#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <vector>


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

		throw std::runtime_error("Failed to compile shader: " + std::string(log.begin(), log.end()));
	}
}

explicit Shader::Shader(const std::string& vertex, const std::string& fragment) {
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

		throw std::runtime_error("Failed to link program: " + std::string(log.begin(), log.end()));
	}
}

Shader::~Shader() {
	glDeleteProgram(m_programId);

	glDeleteShader(m_vertexShaderId);
	glDeleteShader(m_fragmentShaderId);

	m_programId = 0;
	m_vertexShaderId = 0;
	m_fragmentShaderId = 0;
}

void Shader::use() const {
	glUseProgram(m_programId);
}