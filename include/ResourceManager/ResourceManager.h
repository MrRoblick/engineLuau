#pragma once

#include <memory>
#include <ResourceManager/Managers/MeshManager.h>
#include <ResourceManager/Managers/Texture2DManager.h>

class ResourceManager {
public:
	MeshManager& meshManager = MeshManager::getInstance();
	Texture2DManager& texture2DManager = Texture2DManager::getInstance();
	
	static ResourceManager& getInstance();
private:
	ResourceManager() = default;
	~ResourceManager() = default;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
};