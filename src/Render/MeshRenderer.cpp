#include <Render/MeshRenderer.h>
#include <Texture/ITexture.h>
#include <Mesh/IMesh.h>
#include <Shader/Shader.h>
#include <memory>
#include <vector>

#include <iostream>

void MeshRenderer::draw(const IMesh& mesh) {
	glBindVertexArray(mesh.getVAO());
	glDrawElements(GL_TRIANGLES, mesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}


void MeshRenderer::draw(const IMesh& mesh, const std::vector<std::shared_ptr<ITexture>>& textures) {
	glBindVertexArray(mesh.getVAO());
	for (int i = 0; i < textures.size(); i++) {
		textures[i]->bind(i);
	}
	glDrawElements(GL_TRIANGLES, mesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}