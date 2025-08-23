#pragma once

#include <memory>
#include <ResourceManager/Managers/MeshManager.h>

class ResourceManager {
public:
	MeshManager& meshManager = MeshManager::getInstance();
	static ResourceManager& getInstance();
private:
	ResourceManager() = default;
	~ResourceManager() = default;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
};