#include <Render/MeshRenderer.h>
#include <Mesh/IMesh.h>
#include <Shader/Shader.h>
#include <memory>

#include <iostream>

void MeshRenderer::draw(const IMesh& mesh) {
	glBindVertexArray(mesh.getVAO());
	glDrawElements(GL_TRIANGLES, mesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

