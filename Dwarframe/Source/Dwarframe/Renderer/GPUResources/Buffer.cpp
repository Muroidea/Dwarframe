#include "pch.h"
#include "Buffer.h"

namespace Dwarframe {

	Buffer::Buffer(ID3D12Device* Device, size_t SizeInBytes, std::wstring DebugName, D3D12_RESOURCE_STATES InitialState)
		: GPUResourceBase(InitialState, SizeInBytes)
	{
		D3D12_HEAP_PROPERTIES HeapProperties {};
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.CreationNodeMask = 0;
		HeapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC ResourceDesc {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		ResourceDesc.Width = SizeInBytes;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		// TODO: Add logging and handle InitialState
		ThrowIfFailed(Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, InitialState, nullptr, IID_PPV_ARGS(&m_Resource)));
	
	//#ifdef _DEBUG
		m_Resource->SetName(DebugName.c_str());
	//#endif
	}

}