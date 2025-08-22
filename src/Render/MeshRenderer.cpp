#include <Render/MeshRenderer.h>
#include <Mesh/IMesh.h>

void MeshRenderer::draw(const IMesh& mesh) {
	glBindVertexArray(mesh.getVAO());
	glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}