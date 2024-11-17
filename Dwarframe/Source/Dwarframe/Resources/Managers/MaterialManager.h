#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "ResourceManager.h"
#include "Dwarframe/Resources/Material.h"

namespace Dwarframe {

	class MaterialManager : public ResourceManager<Material>
	{
	public:
	#ifdef WITH_EDITOR
		bool Extends(std::string ElementName) override;

		virtual void RenderOptions() override;
		virtual void RenderResourceList(std::vector<ContentBasicInfo>& ResourceList) override;
	#endif

		bool AddResource(std::filesystem::path AssetPath, std::string AssetName) override;

	private:
		
	public:
		MaterialManager();
		MaterialManager(const MaterialManager& Other) = delete;
		MaterialManager& operator=(const MaterialManager& Other) = delete;
		virtual ~MaterialManager() = default;
	};

}

#endif //!MATERIALMANAGER_H