#include "pch.h"
#include "TextureManager.h"

#include "Dwarframe/Renderer/GPUResources/UploadBuffer.h"

#include "ImGUI/imgui.h"
#include <string>

namespace Dwarframe {

	TextureManager::TextureManager()
	{
		m_ConfigFilePath = Paths::GetConfigDir().append("TextureManagerAssets.txt");
	}

	bool TextureManager::AddResource(std::filesystem::path AssetPath, std::string AssetName)
	{
		if (!std::filesystem::exists(AssetPath))
		{
			// Add logging.
			return false;
		}

		if (m_Resources.contains(std::hash<std::string>{}(AssetPath.string())))
		{
			// Add logging.
			return false;
		}

		Texture* NewTexture = new Texture(AssetPath.string(), AssetName);
		if (TextureLoader::Load(AssetPath, NewTexture))
		{
			m_Resources.emplace(NewTexture->m_Hash, NewTexture);
			return true;
		}

		return false;
	}

	void TextureManager::FillTransitionBarrierToNewState(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState)
	{
		
	}

	void TextureManager::SubmitTextureDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer)
	{

	}

	void TextureManager::BindTexture(ID3D12GraphicsCommandList6* CommandList)
	{

	}
	
#ifdef WITH_EDITOR
	bool TextureManager::Extends(std::string ElementName)
	{
		if (ElementName == EditorNames::LeftWindow)
		{
			return true;
		}
	}

	void TextureManager::RenderOptions()
	{
		if (!ImGui::CollapsingHeader("Texture Manager:"))
		{
			return;
		}

        static char NewAssetPath[256] = "";
		ImGui::InputText("NewTexturePath", NewAssetPath, sizeof(NewAssetPath) / sizeof(NewAssetPath[0]), ImGuiInputTextFlags_None);

        static char NewAssetName[256] = "";
		ImGui::InputText("NewTextureName", NewAssetName, sizeof(NewAssetName) / sizeof(NewAssetName[0]), ImGuiInputTextFlags_None);

		static std::string LoadResultInfo = "";
		if (ImGui::Button("LoadTexture"))
		{
			if (AddResource(NewAssetPath, NewAssetName))
			{
				LoadResultInfo = "Texture loaded successfuly.";
			}
			else
			{
				LoadResultInfo = "Something went wrong during loading.";
			}

			memset(NewAssetPath, 0, sizeof(NewAssetPath));
			memset(NewAssetName, 0, sizeof(NewAssetName));
		}
		ImGui::Text(LoadResultInfo.c_str());
	}

	void TextureManager::RenderResourceList(std::vector<ContentBasicInfo>& ResourceList)
	{
		ResourceList.reserve(ResourceList.size() + m_Resources.size());

		for (auto& Entry : m_Resources)
		{
			ResourceList.emplace_back(Entry.second->m_AssetName, "Texture", Entry.second->m_AssetPath, Entry.second->m_AssetSize, Entry.second, Entry.second);
		}
	}
#endif
}