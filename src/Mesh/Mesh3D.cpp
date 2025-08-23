#include <Mesh/Mesh3D.h>
#include <vector>


Mesh3D::Mesh3D(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	m_indexCount = static_cast<GLsizei>(indices.size());
	constexpr GLsizei vertexSize = sizeof(Vertex);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, vertexColor)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, textureCoords)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

Mesh3D::~Mesh3D() {
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_ebo) glDeleteBuffers(1, &m_ebo);

	m_vao = 0;
	m_vbo = 0;
	m_ebo = 0;
	m_indexCount = 0;
}

GLuint Mesh3D::getVAO() const {
	return m_vao;
}

GLsizei Mesh3D::getIndicesCount() const {
	return m_indexCount;
}