#ifndef DESCRIPTORHEAP_H
#define DESCRIPTORHEAP_H

namespace Dwarframe {
	
	class GPUResourceBase;
	class Texture2D;
	class Buffer;

	class DescriptorHeap
	{
	public:
		DescriptorHeap() = default;
		virtual ~DescriptorHeap();

		virtual void Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName) = 0;
		virtual void Shutdown();

		inline ID3D12DescriptorHeap* GetDescriptorHeap() { return m_DescriptorHeap.Get(); }

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(); }
		inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(); }

		inline D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorType() { return m_DescriptorHeap->GetDesc().Type; }
		inline uint32_t GetNumOfDescriptors() { return m_DescriptorHeap->GetDesc().NumDescriptors; }
		inline D3D12_DESCRIPTOR_HEAP_FLAGS GetDescriptorFlags() { return m_DescriptorHeap->GetDesc().Flags; }

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t DescriptorIndex) const 
		{
			if (DescriptorIndex > m_NumOfDescriptors)
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{};
			}

			D3D12_CPU_DESCRIPTOR_HANDLE Result { m_CPUHeapStart };
			Result.ptr += DescriptorIndex * m_IncrementSize;

			return Result;
		}

		inline void ReleaseReourceView(uint16_t Slot)
		{
#ifdef _DEBUG
			if (Slot >= m_NumOfDescriptors)
			{
				__debugbreak();
				return;
			}

			for (uint16_t i : m_FreeSlots)
			{
				if (i == Slot)
				{
					__debugbreak();
					return;
				}
			}
#endif // _DEBUG

			m_FreeSlots.push_back(Slot);
		}

	protected:
		void InitializeInternal(ID3D12Device8* Device, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorType, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName);

		ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPUHeapStart {};
		uint32_t m_IncrementSize = 0;

		uint16_t m_NumOfDescriptors = 0;
		std::vector<uint16_t> m_FreeSlots;
	};

	class RenderTargetDescriptorHeap : public DescriptorHeap
	{
	public:
		virtual void Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName = L"RenderTargetDescriptorHeap") override;

		void AddRenderTargetView(ID3D12Device8* Device, GPUResourceBase* Resource, DXGI_FORMAT Format);
	};

	class DepthStencilDescriptorHeap : public DescriptorHeap
	{
	public:
		virtual void Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName = L"DepthStencilDescriptorHeap") override;

		void AdDepthStencilView(ID3D12Device8* Device, GPUResourceBase* Resource, DXGI_FORMAT Format);
	};

	class ShaderResourceDescriptorHeap : public DescriptorHeap
	{
	public:
		virtual void Initialize(ID3D12Device8* Device, uint32_t NumOfDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorFlags, std::wstring DebugName = L"ShaderResourceDescriptorHeap") override;

		void AddConstantBufferView(ID3D12Device8* Device, Buffer* Resource);
		void AddTexture2DView(ID3D12Device8* Device, Texture2D* Resource, DXGI_FORMAT Format);
		//void AddUnorderedAccessView(ID3D12Device8* Device, GPUResourceBase* Resource, DXGI_FORMAT Format);
	};
}

#endif // !DESCRIPTORTABLE_H