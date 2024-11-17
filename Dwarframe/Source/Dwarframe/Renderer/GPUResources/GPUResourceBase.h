#ifndef GPURESOURCEBASE_H
#define GPURESOURCEBASE_H

namespace Dwarframe {

	class DescriptorHeap;

	class GPUResourceBase
	{
	public:
		inline ID3D12Resource* GetResource() const { return m_Resource.Get(); }
		inline size_t GetBufferSizeInBytes() const { return m_SizeInBytes; }

		inline D3D12_RESOURCE_STATES GetPreviousState() const { return m_PreviousState; }
		inline D3D12_RESOURCE_STATES GetCurrentState() const { return m_CurrentState; }

		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return m_Resource->GetGPUVirtualAddress(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle();

		void FillTransitionBarrierToNewState(D3D12_RESOURCE_BARRIER& Barrier, D3D12_RESOURCE_STATES NewState);
		void FillTransitionBarrierToPreviousState(D3D12_RESOURCE_BARRIER& Barrier);

		void TransitToNewState(ID3D12GraphicsCommandList6* CommandList, D3D12_RESOURCE_STATES NewState);
		void TransitToPreviousState(ID3D12GraphicsCommandList6* CommandList);
		
		void SetCPUHandleInfo(DescriptorHeap* InDescriptorHeap, uint16_t InUsedSlot);
		void SetName(std::wstring NewName);

	protected:
		GPUResourceBase(D3D12_RESOURCE_STATES InitialState, size_t SizeInBytes);
		virtual ~GPUResourceBase();

	protected:
		D3D12_RESOURCE_STATES m_PreviousState;
		D3D12_RESOURCE_STATES m_CurrentState;
		
		ComPtr<ID3D12Resource> m_Resource;

		struct CPUHandleInfo
		{
			DescriptorHeap* m_DescriptorHeap = nullptr;
			uint16_t m_UsedSlot = std::numeric_limits<uint16_t>::max();
		} m_CPUHandleInfo;

		size_t m_SizeInBytes;
	};

}

#endif // !IRESOURCE_H
