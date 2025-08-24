#pragma once
#include <glad/glad.h>
#include <Mesh/IMesh.h>
#include <Texture/ITexture.h>
#include <memory>
#include <vector>

class MeshRenderer {
public:
	static void draw(const IMesh& mesh);
	static void draw(const IMesh& mesh, const std::vector<std::shared_ptr<ITexture>>& textures);
	//static void draw(const std::unique_ptr<IMesh>& mesh, const std::unique_ptr<Shader>& shader);
};