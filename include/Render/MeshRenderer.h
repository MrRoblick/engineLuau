#pragma once
#include <glad/glad.h>
#include <Mesh/IMesh.h>

class MeshRenderer {
public:
	static void draw(const IMesh& mesh);
};