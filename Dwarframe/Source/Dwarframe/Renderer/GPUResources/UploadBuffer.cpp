#include "pch.h"
#include "UploadBuffer.h"

#include "Dwarframe/Renderer/GPUResources/Buffer.h"
#include "Dwarframe/Renderer/GPUResources/Texture2D.h"

namespace Dwarframe {

	UploadBuffer::UploadBuffer(ID3D12Device* Device, uint64 SizeInBytes, std::wstring DebugName)
		: GPUResourceBase(D3D12_RESOURCE_STATE_GENERIC_READ, SizeInBytes), m_MappedRange(), m_MappedMemory(nullptr), m_OccupiedSizeInBytes(0)
	{
		D3D12_HEAP_PROPERTIES HeapProperties {};
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
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

		ThrowIfFailed(Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource)));
		
	//#ifdef _DEBUG
		m_Resource->SetName(DebugName.c_str());
	//#endif
	}

	void UploadBuffer::FlushSubmittedData(ID3D12GraphicsCommandList6* CommandList)
	{
		Map();
		if (!m_MappedMemory) return; 
		uint64 OffsetInBytes = 0;
		for (uint32 Index = 0; Index < m_TexturesToUpload.size(); Index++)
		{
			memcpy(&((char*)m_MappedMemory)[OffsetInBytes], m_TexturesToUpload[Index].Data->GetData(), m_TexturesToUpload[Index].Data->GetSizeInBytes());
			OffsetInBytes += m_TexturesToUpload[Index].Data->GetSizeInBytes();
		}

		for (uint32 Index = 0; Index < m_DataToUpload.size(); Index++)
		{
			memcpy(&((char*)m_MappedMemory)[OffsetInBytes], m_DataToUpload[Index].DataPointer, m_DataToUpload[Index].DataSize);
			OffsetInBytes += m_DataToUpload[Index].DataSize;
		}
		Unmap();

		D3D12_BOX TextureSizeAsBox {};
        TextureSizeAsBox.left = TextureSizeAsBox.top = TextureSizeAsBox.front = 0;
        TextureSizeAsBox.back = 1;

        D3D12_TEXTURE_COPY_LOCATION TextSrcLocation {}, TextDstLocation {};
        TextSrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        TextSrcLocation.PlacedFootprint.Footprint.Depth = 1;
        
        TextDstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        TextDstLocation.SubresourceIndex = 0;
		
		OffsetInBytes = 0;
		for (uint32 Index = 0; Index < m_TexturesToUpload.size(); Index++)
		{
			TextureSizeAsBox.right = m_TexturesToUpload[Index].Data->GetWidth();
			TextureSizeAsBox.bottom = m_TexturesToUpload[Index].Data->GetHeight();
			
			TextSrcLocation.pResource = m_Resource.Get();
			TextSrcLocation.PlacedFootprint.Offset = OffsetInBytes;
			TextSrcLocation.PlacedFootprint.Footprint.Width = m_TexturesToUpload[Index].Data->GetWidth();
			TextSrcLocation.PlacedFootprint.Footprint.Height = m_TexturesToUpload[Index].Data->GetHeight();
			TextSrcLocation.PlacedFootprint.Footprint.RowPitch = m_TexturesToUpload[Index].Data->GetStride();
			TextSrcLocation.PlacedFootprint.Footprint.Format = m_TexturesToUpload[Index].Data->GetPixelFormat();

			TextDstLocation.pResource = m_TexturesToUpload[Index].Resource->GetResource();

			CommandList->CopyTextureRegion(&TextDstLocation, 0, 0, 0, &TextSrcLocation, &TextureSizeAsBox);
			OffsetInBytes += m_TexturesToUpload[Index].Data->GetSizeInBytes();
		}

		for (uint32 Index = 0; Index < m_DataToUpload.size(); Index++)
		{
			CommandList->CopyBufferRegion(m_DataToUpload[Index].Resource->GetResource(), 0, m_Resource.Get(), OffsetInBytes, m_DataToUpload[Index].DataSize);
			OffsetInBytes += m_DataToUpload[Index].DataSize;
		}

		m_TexturesToUpload.clear();
		m_DataToUpload.clear();
		m_OccupiedSizeInBytes = 0;
	}
}
