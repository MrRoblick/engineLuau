#pragma once
#include <memory>
#include <Mesh/Mesh3D.h>
#include <string>

class GlbDeserializer {
public:
	GlbDeserializer() = default;
	~GlbDeserializer() = default;
	static std::pair<std::vector<Mesh3D::Vertex>, std::vector<uint32_t>> deserialize(const std::string& filename);
};