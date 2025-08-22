#pragma once
#include <glad/glad.h>
#include <Mesh/IMesh.h>

#include <Shader/Shader.h>
#include <memory>


class MeshRenderer {
public:
	static void draw(const IMesh& mesh);
	//static void draw(const std::unique_ptr<IMesh>& mesh, const std::unique_ptr<Shader>& shader);
};