#include "pch.h"
#include "MaterialManager.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

	MaterialManager::MaterialManager()
	{
		m_ConfigFilePath = Paths::GetConfigDir().append("MaterialManagerAssets.txt");
	}

	bool MaterialManager::AddResource(std::filesystem::path AssetPath, std::string AssetName)
	{
		if (m_Resources.contains(std::hash<std::string>{}(AssetPath.string())))
		{
			// Add logging.
			return false;
		}

		Material* NewMaterial = new Material(AssetPath.string(), AssetName);
		m_Resources.emplace(NewMaterial->m_Hash, NewMaterial);
		// TODO: remove it
		NewMaterial->LoadAsset();
		return true;
	}
	
#ifdef WITH_EDITOR
	bool MaterialManager::Extends(std::string ElementName)
	{
		return true;
	}

	void MaterialManager::RenderOptions()
	{
		if (!ImGui::CollapsingHeader("Material Manager:"))
		{
			return;
		}

        static char NewAssetName[256] = "";
		ImGui::InputText("NewMaterialName", NewAssetName, sizeof(NewAssetName) / sizeof(NewAssetName[0]), ImGuiInputTextFlags_None);

		static std::string LoadResultInfo = "";
		if (ImGui::Button("AddMaterial"))
		{
			if (AddResource(Paths::GetResourcesDir().append(NewAssetName), NewAssetName))
			{
				LoadResultInfo = "Material created successfuly.";
			}
			else
			{
				LoadResultInfo = "Something went wrong during creation process.";
			}

			memset(NewAssetName, 0, sizeof(NewAssetName));
		}
		ImGui::Text(LoadResultInfo.c_str());
	}

	void MaterialManager::RenderResourceList(std::vector<ContentBasicInfo>& ResourceList)
	{
		ResourceList.reserve(ResourceList.size() + m_Resources.size());

		for (auto& Entry : m_Resources)
		{
			ResourceList.emplace_back(Entry.second->m_AssetName, "Material", Entry.second->m_AssetPath, Entry.second->m_AssetSize, Entry.second, Entry.second);
		}
	}
#endif
}