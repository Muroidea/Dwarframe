#ifndef UPLOADBUFFER_H
#define UPLOADBUFFER_H

#include "Dwarframe/Renderer/GPUResources/GPUResourceBase.h"
#include "Dwarframe/Resources/Texture.h"

namespace Dwarframe {

	class Buffer;
	class Texture2D;

	class UploadBuffer : public GPUResourceBase
	{
	public:
		UploadBuffer(ID3D12Device* Device, uint64 SizeInBytes, std::wstring DebugName = L"UploadBuffer");
		
		inline uint64 GetBufferSize() const
		{
			return m_SizeInBytes;
		}

		inline bool AnythingToUpload() const
		{
			return  !m_DataToUpload.empty() || !m_TexturesToUpload.empty();
		}

		inline void UploadData(void* DataPtr, uint64 SizeInBytes, uint64 OffsetInBytes = 0)
		{
			memcpy(m_MappedMemory, ((char**)&DataPtr)[OffsetInBytes], SizeInBytes);
		}

		inline void Map(uint64 RangeStart = 0, uint64 RangeEnd = 0)
		{
			if (m_MappedMemory)
			{
				// Buffer need do get unmapped before can be mapped again
				return;
			}

			m_MappedRange.Begin = RangeStart;
			m_MappedRange.End = RangeEnd;
			
			ThrowIfFailed(m_Resource->Map(0, &m_MappedRange, &m_MappedMemory));
		}

		inline void Map(void** BufferAddressPtr, uint64 RangeStart = 0, uint64 RangeEnd = 0)
		{
			if (m_MappedMemory)
			{
				// Buffer need do get unmapped before can be mapped again
				return;
			}

			m_MappedRange.Begin = RangeStart;
			m_MappedRange.End = RangeEnd;
			
			ThrowIfFailed(m_Resource->Map(0, &m_MappedRange, BufferAddressPtr));
			m_MappedMemory = BufferAddressPtr;
		}

		inline void Unmap()
		{
			m_Resource->Unmap(0, &m_MappedRange);
			m_MappedMemory = nullptr;
		}

		bool SubmitDataToUpload(Buffer* Resource, void* DataPointer, uint64 DataSize)
		{
			if (m_SizeInBytes >= m_OccupiedSizeInBytes + DataSize)
			{
				m_DataToUpload.emplace_back((GPUResourceBase*)Resource, DataPointer, DataSize);
				m_OccupiedSizeInBytes += DataSize;

				return true;
			}

			return false;
		}

		inline bool SubmitTextureToUpload(Texture2D* Resource, Texture* Data)
		{
			if (m_SizeInBytes >= m_OccupiedSizeInBytes + Data->GetSizeInBytes())
			{
				m_TexturesToUpload.emplace_back((GPUResourceBase*)Resource, Data);
				m_OccupiedSizeInBytes += Data->GetSizeInBytes();

				return true;
			}

			return false;
		}

		void FlushSubmittedData(ID3D12GraphicsCommandList6* CommandList);

	protected:
		struct DataToUpload
		{
			GPUResourceBase* Resource;
			void* DataPointer;
			uint64 DataSize;
		};
		
		struct TextureToUpload
		{
			GPUResourceBase* Resource;
			Texture* Data;
		};

		D3D12_RANGE m_MappedRange;
		uint64 m_OccupiedSizeInBytes;
		void* m_MappedMemory;

		std::vector<DataToUpload> m_DataToUpload;
		std::vector<TextureToUpload> m_TexturesToUpload;
	};
}

#endif
