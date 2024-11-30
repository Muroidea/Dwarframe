#ifndef DXCONTEXT_H
#define DXCONTEXT_H

#include "Dwarframe/Core.h"
#include "Dwarframe/Helpers/TextureLoader.h"
#include "Dwarframe/Renderer/Pipeline/RootSignature.h"
#include "Dwarframe/Renderer/Pipeline/PipelineState.h"
#include "Dwarframe/Renderer/Pipeline/DescriptorHeap.h"
#include "Dwarframe/Renderer/Pipeline/MaterialProxy.h"

namespace Dwarframe {
	
	class Buffer;
	class Texture2D;
	class UploadBuffer;
	class Shader;

	class DXContext 
	{
	public:
		static constexpr uint64 s_NumOfFrames = 2;

		void Initialize();
		void Shutdown();

		void Update(float32 DeltaTime);

		void LoadData();

		// Synchronization interface
		inline uint64 Signal();
		inline bool IsFenceComplete(uint64 FenceValue);
		inline void WaitForFenceValue(uint64 FenceValue);
		inline void Flush();

		inline ID3D12Device8* GetDevice() const { return m_D3D12Device.Get(); }
		inline ID3D12GraphicsCommandList6* GetCommandList() const { return m_D3D12CommandList.Get(); }

		inline void ResetCommandList() const
		{
			m_D3D12CommandAllocator->Reset();
			m_D3D12CommandList->Reset(m_D3D12CommandAllocator.Get(), nullptr);		
		}

		inline void DispatchCommandQueue()
		{
			ThrowIfFailed(m_D3D12CommandList->Close(), "Failed at closing command list");

			ID3D12CommandList* Lists[] = { m_D3D12CommandList.Get() };
			m_D3D12CommandQueue->ExecuteCommandLists(1, Lists);
			Flush();
		}

	private:
		struct Correction
		{
			float32 R, G, B;
			float32 AspectRatio;
		};

		static const uint32 NumOfCubes = 3;
		static const uint32 NumOfBuddhas = 1;
		static constexpr uint32 NumOfInstances = NumOfCubes + NumOfBuddhas;
		XMMATRIX m_ModelMatrices[NumOfInstances];
		XMMATRIX m_ViewMatrix;
		XMMATRIX m_ProjectionMatrix;

		struct MeshConstants
		{
			XMMATRIX World;
			XMMATRIX WorldView;
			XMMATRIX WorldViewProj;
			uint32 DrawMeshlets;
		} m_MeshConstants;

		//TextureLoader::TextureData m_TextureData;
		float32 Scale;
		float32 RotationAngle;
		float32 Direction = 1.0f;
		
		//Mesh m_BuddhaMesh;
		//Model m_Model;
		
		MaterialProxy m_BaseMeshMaterial;

		RootSignature m_ClassicRootSignature;
		Shader* VertexShader = nullptr;
		Shader* PixelShader = nullptr;
		Shader* RootSignatureShader = nullptr;

		UploadBuffer* m_UploadBuffer;
		Buffer* m_CorrectionConstantBuffer;
		Buffer* m_MeshConstantBuffer;
		Texture2D* m_BrickTexture;

		ShaderResourceDescriptorHeap m_SRV_CBV_DescriptorHeap;

		MeshPipelineState m_MeshPipeline;

		ComPtr<ID3D12Device8> m_D3D12Device;

		ComPtr<ID3D12CommandQueue> m_D3D12CommandQueue;
		ComPtr<ID3D12Fence1> m_D3D12Fence;
		UINT m_FenceValue = 0;
		HANDLE m_FenceEvent = nullptr;


		ComPtr<ID3D12CommandAllocator> m_D3D12CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList6> m_D3D12CommandList;

	public:
		DXContext(const DXContext& Other) = delete; 
		DXContext& operator=(const DXContext& Other) = delete; 

		inline static DXContext& Get()
		{
			static DXContext Instance;

			return Instance;
		}

		inline static ID3D12Device8* Device()
		{
			return Get().m_D3D12Device.Get();
		}

		inline static ID3D12GraphicsCommandList6* CommandList()
		{
			return Get().m_D3D12CommandList.Get();
		}

	private:
		DXContext() = default;
	};

}

#endif // !DXCONTEXT_H
