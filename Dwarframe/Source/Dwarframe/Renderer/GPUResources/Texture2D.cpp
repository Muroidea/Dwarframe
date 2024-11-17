#include "pch.h"
#include "Texture2D.h"

namespace Dwarframe {

	Texture2D::Texture2D(ID3D12Device* Device, const Texture* TextureData, std::wstring DebugName)
		: GPUResourceBase(D3D12_RESOURCE_STATE_COPY_DEST, TextureData->GetSizeInBytes())
	{
		D3D12_HEAP_PROPERTIES HeapProperties {};
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.CreationNodeMask = 0;
		HeapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC ResourceDesc {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		ResourceDesc.Width = TextureData->GetWidth();
		ResourceDesc.Height = TextureData->GetHeight();
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1; // TODO: Check what value is proper here
		ResourceDesc.Format = TextureData->GetPixelFormat();
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		// TODO: Add logging and handle failure
		ThrowIfFailed(Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Resource)));
	
	//#ifdef _DEBUG
		m_Resource->SetName(DebugName.c_str());
	//#endif
	}

}