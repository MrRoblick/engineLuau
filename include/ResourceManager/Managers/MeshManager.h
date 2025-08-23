#pragma once

#include <Mesh/Mesh3D.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class MeshManager {
public:
	static MeshManager& getInstance();
	/* SUPPORTED ONLY GLB */
	std::shared_ptr<Mesh3D> loadMeshFromFile(const std::string& filename);
	uint32_t count();
private:
	MeshManager() = default;
	~MeshManager() = default;

	MeshManager(const MeshManager&) = delete;
	MeshManager& operator=(const MeshManager&) = delete;

	std::unordered_map<std::string, std::shared_ptr<Mesh3D>> m_meshes;
};