#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Dwarframe/Renderer/RenderPasses/RenderPass.h"
#include "Dwarframe/Renderer/GPUResources/GPUResourceBase.h"
#include "Dwarframe/Renderer/Pipeline/DescriptorHeap.h"
#include "Dwarframe/Renderer/Pipeline/PipelineState.h"
#include "Dwarframe/Resources/Mesh.h"

#include "Dwarframe/Editor/EditorExtender.h"

namespace Dwarframe {

	class Buffer;
	class UploadBuffer;

	class BaseRenderPass : public RenderPass, public IEditorExtender
	{
	public:
		BaseRenderPass(std::string Name = "BasicRenderPass");
		~BaseRenderPass();
		
		virtual void AddRenderable(Renderable* InRenderable) override;
		virtual void RemoveRenderable(Renderable* InRenderable) override;

		// Inherited via RenderPass
		virtual void Render() override;
		virtual void Update() override;

		void SetUploadBufferSize(uint32 NewSize);
		void RecreateUploadBuffer();

		inline void SetViewMatrix(XMMATRIX ViewMatrix) { m_ViewMatrix = ViewMatrix; }
		inline void SetProjectionMatrix(XMMATRIX ProjectionMatrix) { m_ProjectionMatrix = ProjectionMatrix; }

#if WITH_EDITOR
		// Inherited via IEditorExtender
		virtual bool Extends(std::string_view ElementName) override;
		virtual void RenderOptions() override;
#endif

	private:
		void AdjustBarriers(uint32 NewElements); // Make sure that we have enough space for given number of barriers
		void FillTransitionBarrierToNewState_UnSafe(GPUResourceBase* Resource, D3D12_RESOURCE_STATES NewState);
		void ResourceBarrier();

		void FillTransitionBarrierToNewStateMeshlets(const Submesh& InSubmesh, D3D12_RESOURCE_STATES NewState);
		void SubmitMeshletDataToUpload(Submesh& InSubmesh);
		void BindMeshletData(uint32 BufferID);

	private:
		uint32 m_ShadingVariant = 0;

		bool bEnableProjectionMatrix = true;
		float32 FieldOfView = 90.0f;

		bool bEnableViewMatrix = true;
		XMVECTOR EyePosition = XMVectorSet(0.0f, 8.0f, -10.0f, 1.0f);
		XMVECTOR TargetPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR UpDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	private:
		inline static constexpr int32 MaxNumOfPrimitives = 128;

		struct alignas(64) MeshConstants
		{
			XMFLOAT4X4 World[MaxNumOfPrimitives];
			XMFLOAT4X4 WorldView[MaxNumOfPrimitives];
			XMFLOAT4X4 WorldViewProj[MaxNumOfPrimitives];
			uint32 DrawMeshlets;
			/*
			XMFLOAT4X4 World;
			XMFLOAT4X4 WorldView;
			XMFLOAT4X4 WorldViewProj;
			uint32 DrawMeshlets;
			float32 Fill[15];*/
		} m_MeshConstants;

		Buffer* m_MeshConstantsBuffer;
		ShaderResourceDescriptorHeap m_SRV_CBV_DescriptorHeap;

	private:
		uint32 m_UploadBufferSize = MEM_MiB(32);

		std::vector<Renderable*> m_PrimitivesToUpload;

		UploadBuffer* m_UploadBuffer;
		std::vector<D3D12_RESOURCE_BARRIER> m_Barriers;
		uint32 m_LastFreeBarrier = 0;

		std::vector<MeshPipelineState> m_PipelineStates;
		std::vector<Buffer*> m_MeshVerticesBuffer;
		std::vector<Buffer*> m_MeshletTrianglesBuffer;
		std::vector<Buffer*> m_MeshletVertexIndicesBuffer;
		std::vector<Buffer*> m_MeshletsBuffer;

		bool bUpdateTransforms = true;
	};
}

#endif // !TEXTURE2D_H
