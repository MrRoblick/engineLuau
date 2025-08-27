#pragma once

#include <memory>
#include <ResourceManager/Managers/MeshManager.h>
#include <ResourceManager/Managers/Texture2DManager.h>
#include <ResourceManager/Managers/TextureCubeMapManager.h>

class ResourceManager {
public:
	MeshManager& meshManager = MeshManager::getInstance();
	Texture2DManager& texture2DManager = Texture2DManager::getInstance();
	TextureCubeMapManager& textureCubeMapManager = TextureCubeMapManager::getInstance();

	static ResourceManager& getInstance();
private:
	ResourceManager() = default;
	~ResourceManager() = default;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
};