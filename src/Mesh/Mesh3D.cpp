#include <Mesh/Mesh3D.h>
#include <vector>


Mesh3D::Mesh3D(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	m_indexCount = static_cast<GLsizei>(vertices.size());

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

Mesh3D::~Mesh3D() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);

	m_vao = 0;
	m_vbo = 0;
	m_ebo = 0;
	m_indexCount = 0;
}

GLuint Mesh3D::getVAO() const {
	return m_vao;
}

GLsizei Mesh3D::getIndexCount() const {
	return m_indexCount;
}