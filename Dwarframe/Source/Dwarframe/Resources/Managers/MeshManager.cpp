#include "pch.h"
#include "MeshManager.h"

#include "Dwarframe/Renderer/GPUResources/UploadBuffer.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

	MeshManager::MeshManager()
	{
		m_ConfigFilePath = Paths::GetConfigDir().append("MeshManagerAssets.txt");
	}

	bool MeshManager::AddResource(std::filesystem::path AssetPath, std::string AssetName)
	{
		if (!std::filesystem::exists(AssetPath))
		{
			// TODO: Add logging.
			return false;
		}

		if (m_Resources.contains(std::hash<std::string>{}(AssetPath.string())))
		{
			// TODO: Add logging.
			return false;
		}

		Mesh* NewMesh = new Mesh(AssetPath.string(), AssetName);
		if (MeshLoader::Load(AssetPath, NewMesh, false))
		//if (MeshLoader::LoadCube(NewMesh))
		{
			//MeshLoader::BuildMeshlets(NewMesh);
			MeshLoader::BuildMeshletsWithCheck(NewMesh);
			m_Resources.emplace(NewMesh->m_Hash, NewMesh);
			return true;
		}

		return false;
	}

	void MeshManager::FillTransitionBarrierToNewStateMeshletsData(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState)
	{
		
	}

	void MeshManager::SubmitMeshletDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer)
	{
	/*
		size_t SizeInBytes;
		std::wstring DebugName;
		Submesh& Submesh = m_Submeshes[0];

		SizeInBytes = GetVerticesDataSizeInBytes(0);
		DebugName = L"VerticesBuffer";
		Submesh.m_VerticesBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_VerticesBuffer, Submesh.Vertices.data(), SizeInBytes);

		SizeInBytes = GetMeshletTrianglesSizeInBytes(0);
		DebugName = L"MeshletTrianglesBuffer";
		Submesh.m_MeshletTrianglesBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_MeshletTrianglesBuffer, Submesh.m_MeshletTriangles.data(), SizeInBytes);

		SizeInBytes = GetUniqueIndicesSizeInBytes(0);
		DebugName = L"VertexIndicesBuffer";
		Submesh.m_VertexIndicesBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_VertexIndicesBuffer, Submesh.m_VertexIndices.data(), SizeInBytes);

		SizeInBytes = GetMeshletsSizeInBytes(0);
		DebugName = L"MeshletsBuffer";
		Submesh.m_MeshletsBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_MeshletsBuffer, Submesh.m_Meshlets.data(), SizeInBytes);
	*/
	}

	void MeshManager::BindMeshletData(ID3D12GraphicsCommandList6* CommandList)
	{

	}
	
#ifdef WITH_EDITOR
	bool MeshManager::Extends(std::string ElementName)
	{
		if (ElementName == EditorNames::LeftWindow)
		{
			return true;
		}
	}

	void MeshManager::RenderOptions()
	{
		if (!ImGui::CollapsingHeader("Mesh Manager:"))
		{
			return;
		}

        static char NewAssetPath[256] = "";
		ImGui::InputText("NewMeshPath", NewAssetPath, sizeof(NewAssetPath) / sizeof(NewAssetPath[0]), ImGuiInputTextFlags_None);

        static char NewAssetName[256] = "";
		ImGui::InputText("NewAssetName", NewAssetName, sizeof(NewAssetName) / sizeof(NewAssetName[0]), ImGuiInputTextFlags_None);

		static std::string LoadResultInfo = "";
		if (ImGui::Button("LoadMesh"))
		{
			if (AddResource(NewAssetPath, NewAssetName))
			{
				LoadResultInfo = "Mesh loaded successfully.";
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

	void MeshManager::RenderResourceList(std::vector<ContentBasicInfo>& ResourceList)
	{
		ResourceList.reserve(ResourceList.size() + m_Resources.size());

		for (auto& Entry : m_Resources)
		{
			ResourceList.emplace_back(Entry.second->m_AssetName, "Mesh", Entry.second->m_AssetPath, Entry.second->m_AssetSize, Entry.second, Entry.second);
		}
	}
#endif
}