#pragma once
#include <Texture/Texture2D.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class Texture2DManager {
public:
	static Texture2DManager& getInstance();
	/* SUPPORTED ONLY GLB */
	std::shared_ptr<Texture2D> loadTextureFromFile(const std::string& filename);
	uint32_t count();
private:
	Texture2DManager() = default;
	~Texture2DManager() = default;

	Texture2DManager(const Texture2DManager&) = delete;
	Texture2DManager& operator=(const Texture2DManager&) = delete;

	std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
};