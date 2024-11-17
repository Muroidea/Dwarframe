#include "pch.h"
#include "DescriptorHeap.h"

#include "Dwarframe/Renderer/GPUResources/Buffer.h"
#include "Dwarframe/Renderer/GPUResources/Texture2D.h"

namespace Dwarframe {

	DescriptorHeap::~DescriptorHeap()
	{
		if (m_DescriptorHeap)
		{
			m_DescriptorHeap.Reset();
		}
	}

	void DescriptorHeap::Shutdown()
	{
		m_DescriptorHeap.Reset();
		m_CPUHeapStart = {};
		m_IncrementSize = 0;

		m_NumOfDescriptors = 0;
		m_FreeSlots.clear();
	}

	void DescriptorHeap::InitializeInternal(ID3D12Device8* Device, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorType, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName)
	{
		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
		DescriptorHeapDesc.Type = DescriptorType;
		DescriptorHeapDesc.NumDescriptors = NumOfDescriptors;
		DescriptorHeapDesc.Flags = DescriptorFlags;
		DescriptorHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));
		m_DescriptorHeap->SetName(DebugName.c_str());

		m_NumOfDescriptors = NumOfDescriptors;
		m_CPUHeapStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_IncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorType);

		m_FreeSlots.reserve(m_NumOfDescriptors);
		for (uint16_t i = m_NumOfDescriptors; i > 0; i--)
		{
			m_FreeSlots.push_back(i - 1);
		}
	}

	void RenderTargetDescriptorHeap::Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName /*= L"RenderTargetDescriptorHeap"*/)
	{
		InitializeInternal(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NumOfDescriptors, DescriptorFlags, DebugName);
	}

	void RenderTargetDescriptorHeap::AddRenderTargetView(ID3D12Device8* Device, GPUResourceBase* Resource, DXGI_FORMAT Format)
	{
		if (m_FreeSlots.size() == 0)
		{
			return;
		}

		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc{};
		RTVDesc.Format = Format;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;

		uint16_t Slot = m_FreeSlots.back();
		m_FreeSlots.pop_back();
		
		Device->CreateRenderTargetView(Resource->GetResource(), &RTVDesc, GetCPUDescriptorHandle(Slot));
		Resource->SetCPUHandleInfo(this, Slot);
	}

	void DepthStencilDescriptorHeap::Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName /*= L"DepthStencilDescriptorHeap"*/)
	{
		InitializeInternal(Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, NumOfDescriptors, DescriptorFlags, DebugName);
	}

	void DepthStencilDescriptorHeap::AdDepthStencilView(ID3D12Device8* Device, GPUResourceBase* Resource, DXGI_FORMAT Format)
	{
#ifdef _DEBUG
		if (m_FreeSlots.size() == 0)
		{
			return;
		}
#endif

		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
		DSVDesc.Format = Format;
		DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;
		DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

		uint16_t Slot = m_FreeSlots.back();
		m_FreeSlots.pop_back();
		
		Device->CreateDepthStencilView(Resource->GetResource(), &DSVDesc, GetCPUDescriptorHandle(Slot));
		Resource->SetCPUHandleInfo(this, Slot);
	}

	void ShaderResourceDescriptorHeap::Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName /*= L"ShaderResourceDescriptorHeap"*/)
	{
		InitializeInternal(Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, NumOfDescriptors, DescriptorFlags, DebugName);
	}
	
	void ShaderResourceDescriptorHeap::AddConstantBufferView(ID3D12Device8* Device, Buffer* Resource)
	{
#ifdef _DEBUG
		if (m_FreeSlots.size() == 0)
		{
			return;
		}
#endif

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc{};
		CBVDesc.BufferLocation = Resource->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = GetSizeAligned(sizeof(Resource->GetBufferSizeInBytes()), (size_t)256);

		uint16_t Slot = m_FreeSlots.back();
		m_FreeSlots.pop_back();
		
		Device->CreateConstantBufferView(&CBVDesc, GetCPUDescriptorHandle(Slot));
		Resource->SetCPUHandleInfo(this, Slot);
	}
	
	void ShaderResourceDescriptorHeap::AddTexture2DView(ID3D12Device8* Device, Texture2D* Resource, DXGI_FORMAT Format)
	{
#ifdef _DEBUG
		if (m_FreeSlots.size() == 0)
		{
			return;
		}
#endif

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = Format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.PlaneSlice = 0;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		uint16_t Slot = m_FreeSlots.back();
		m_FreeSlots.pop_back();
		
		Device->CreateShaderResourceView(Resource->GetResource(), &SRVDesc, GetCPUDescriptorHandle(Slot));
		Resource->SetCPUHandleInfo(this, Slot);
	}
}