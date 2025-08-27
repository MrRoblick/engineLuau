#pragma once

#include <Mesh/IMesh.h>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class DynamicMesh3D : public IMesh {
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
	explicit DynamicMesh3D();
	explicit DynamicMesh3D(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~DynamicMesh3D();
	GLuint getVAO() const override;
	GLsizei getIndicesCount() const override;

	void updateVBO(const std::vector<Vertex>& vertices) const;
	void updateEBO(const std::vector<unsigned int>& indices) const;
};