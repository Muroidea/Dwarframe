#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Dwarframe/Resources/GameAsset.h"

namespace Dwarframe {

	class ResourceManagerBase : public IEditorExtender
	{
	public:
		ResourceManagerBase(const ResourceManagerBase& Other) = delete;
		ResourceManagerBase& operator=(const ResourceManagerBase& Other) = delete;
		virtual ~ResourceManagerBase() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

	protected:
		ResourceManagerBase() = default;
	};
	
	template <typename Derived>
	concept DerivedType = std::is_base_of<GameAsset, Derived>::value;

	template<DerivedType Type>
	class ResourceManager : public ResourceManagerBase
	{
	public:
		ResourceManager(const ResourceManager& Other) = delete;
		ResourceManager& operator=(const ResourceManager& Other) = delete;
		virtual ~ResourceManager() = default;
		
		virtual bool AddResource(std::filesystem::path AssetPath, std::string AssetName) = 0;

		virtual void Initialize() override;
		virtual void Shutdown() override;

	protected:
		ResourceManager() = default;

		void SaveResourceList();
		void LoadResourceList();

	protected:
		std::filesystem::path m_ConfigFilePath;
		std::unordered_map<HashValue, Type*> m_Resources;
	};

	template<DerivedType Type>
	inline void ResourceManager<Type>::Initialize()
	{
		LoadResourceList();
	}

	template<DerivedType Type>
	inline void ResourceManager<Type>::Shutdown()
	{
		SaveResourceList();
	}

	template<DerivedType Type>
	inline void ResourceManager<Type>::SaveResourceList()
	{
		std::ofstream OutFile (m_ConfigFilePath, std::ios::out);

		if (OutFile.is_open())
		{
			for (std::pair<HashValue, Type*> Entry : m_Resources)
			{
				OutFile << Entry.second->GetAssetName() << "\n";
				OutFile << Entry.second->GetAssetPath() << "\n";
			}
		}
	}

	template<DerivedType Type>
	inline void ResourceManager<Type>::LoadResourceList()
	{
		if (!std::filesystem::exists(m_ConfigFilePath))
		{
			// Nothing to load (no file exist) or wrong path
		}
		
		std::ifstream InFile (m_ConfigFilePath, std::ios::in);

		if (InFile.is_open())
		{
			for (std::string AssetName, AssetPath; InFile >> AssetName >> AssetPath;)
			{
				if (AddResource(AssetPath, AssetName))
				{
				}
			}
		}
	}
}

#endif