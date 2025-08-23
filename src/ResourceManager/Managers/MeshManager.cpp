#include <ResourceManager/Managers/MeshManager.h>
#include <memory>
#include <string>
#include <Mesh/Mesh3D.h>
#include <MeshDeserializer/GlbDeserializer.h>
#include <unordered_map>

MeshManager& MeshManager::getInstance() {
	static MeshManager instance;
	return instance;
}

std::shared_ptr<Mesh3D> MeshManager::loadMeshFromFile(const std::string& filename) {
	auto it = m_meshes.find(filename);
	if (it != m_meshes.end()) {
		return it->second;
	}

	const auto [vertices, indices] = GlbDeserializer::deserialize(filename);
	auto mesh = std::make_shared<Mesh3D>(vertices, indices);
	m_meshes[filename] = mesh;

	return mesh;
}

uint32_t MeshManager::count() {
	return m_meshes.size();
}