#include <ResourceManager/Managers/TextureCubeMapManager.h>
#include <Texture/TextureCubeMap.h>
#include <string>

TextureCubeMapManager& TextureCubeMapManager::getInstance()
{
	static TextureCubeMapManager instance;
	return instance;
}

std::shared_ptr<TextureCubeMap> TextureCubeMapManager::loadTexturesFromFile(const std::string& name,
	const std::string& frontFilename,
	const std::string& backFilename,
	const std::string& leftFilename,
	const std::string& rightFilename,
	const std::string& topFilename,
	const std::string& bottomFilename
)
{
	auto it = m_cubeMaps.find(name);
	if (it != m_cubeMaps.end()) {
		return it->second;
	}

	auto tex = std::make_shared<TextureCubeMap>(frontFilename, backFilename, leftFilename, rightFilename, topFilename, bottomFilename);
	m_cubeMaps[name] = tex;

	return tex;
}

uint32_t TextureCubeMapManager::count()
{
	return m_cubeMaps.size();
}
