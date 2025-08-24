#include <ResourceManager/Managers/Texture2DManager.h>
#include <memory>
#include <string>
#include <Texture/Texture2D.h>
#include <MeshDeserializer/GlbDeserializer.h>
#include <unordered_map>

Texture2DManager& Texture2DManager::getInstance() {
	static Texture2DManager instance;
	return instance;
}

std::shared_ptr<Texture2D> Texture2DManager::loadTextureFromFile(const std::string& filename) {
	auto it = m_textures.find(filename);
	if (it != m_textures.end()) {
		return it->second;
	}
	auto tex = std::make_shared<Texture2D>(filename);
	m_textures[filename] = tex;
	return tex;
}

uint32_t Texture2DManager::count() {
	return m_textures.size();
}