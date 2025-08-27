#pragma once
#include <Texture/TextureCubeMap.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class TextureCubeMapManager {
public:
	static TextureCubeMapManager& getInstance();
	std::shared_ptr<TextureCubeMap> loadTexturesFromFile(const std::string& name,
		const std::string& frontFilename,
		const std::string& backFilename,
		const std::string& leftFilename,
		const std::string& rightFilename,
		const std::string& topFilename,
		const std::string& bottomFilename
	);
	uint32_t count();
private:
	TextureCubeMapManager() = default;
	~TextureCubeMapManager() = default;

	TextureCubeMapManager(const TextureCubeMapManager&) = delete;
	TextureCubeMapManager& operator=(const TextureCubeMapManager&) = delete;

	std::unordered_map<std::string, std::shared_ptr<TextureCubeMap>> m_cubeMaps;
};