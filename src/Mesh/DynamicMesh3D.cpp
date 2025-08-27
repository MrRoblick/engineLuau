#include <Mesh/DynamicMesh3D.h>
#include <vector>
#include <glad/glad.h>

DynamicMesh3D::DynamicMesh3D() : m_vao(0), m_vbo(0), m_ebo(0), m_indexCount(0) {}

DynamicMesh3D::DynamicMesh3D(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	m_indexCount = static_cast<GLsizei>(indices.size());
	constexpr GLsizei vertexSize = sizeof(Vertex);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, vertexColor)));
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, textureCoords)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

GLuint DynamicMesh3D::getVAO() const {
	return m_vao;
}
DynamicMesh3D::~DynamicMesh3D() {
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_ebo) glDeleteBuffers(1, &m_ebo);

	m_vao = 0;
	m_vbo = 0;
	m_ebo = 0;
	m_indexCount = 0;
}
GLsizei DynamicMesh3D::getIndicesCount() const {
	return m_indexCount;
}

void DynamicMesh3D::updateVBO(const std::vector<Vertex>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DynamicMesh3D::updateEBO(const std::vector<unsigned int>& indices) const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}