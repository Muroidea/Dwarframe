#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "ResourceManager.h"
#include "Dwarframe/Resources/Mesh.h"

namespace Dwarframe {

	class Buffer;
	class UploadBuffer;

	class MeshManager : public ResourceManager<Mesh>
	{
	public:
#if WITH_EDITOR
		bool Extends(std::string_view ElementName) override;

		virtual void RenderOptions() override;
		virtual void RenderResourceList(std::vector<ContentBasicInfo>& ResourceList) override;
#endif

		bool AddResource(std::filesystem::path AssetPath, std::string AssetName) override;

		void FillTransitionBarrierToNewStateMeshletsData(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState);
		void SubmitMeshletDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer);
		void BindMeshletData(ID3D12GraphicsCommandList6* CommandList);

	private:
		/*
		std::vector<Buffer*> m_VerticesBuffer;
		std::vector<Buffer*> m_MeshletTrianglesBuffer;
		std::vector<Buffer*> m_VertexIndicesBuffer;
		std::vector<Buffer*> m_MeshletsBuffer;
		*/
		
	public:
		MeshManager();
		MeshManager(const MeshManager& Other) = delete;
		MeshManager& operator=(const MeshManager& Other) = delete;
		virtual ~MeshManager() = default;
	};

}

#endif