#pragma once
#include <glad/glad.h>
#include <vector>
#include <Mesh/IMesh.h>
#include <glm/glm.hpp>



class Mesh3D: public IMesh {
private:
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	GLsizei m_indexCount = 0;
public:
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 vertexColor;
		glm::vec2 textureCoords;
	};
	explicit Mesh3D(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~Mesh3D();
	GLuint getVAO() const override;
	GLsizei getIndicesCount() const override;
};