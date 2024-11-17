#include "pch.h"
#include "GPUResourceBase.h"

#include "Dwarframe/Renderer/Pipeline/DescriptorHeap.h"

namespace Dwarframe {

	GPUResourceBase::GPUResourceBase(D3D12_RESOURCE_STATES InitialState, size_t SizeInBytes)
		: m_CurrentState(InitialState), m_PreviousState(InitialState), m_SizeInBytes(SizeInBytes)
	{
	}

	GPUResourceBase::~GPUResourceBase()
	{
		if (m_CPUHandleInfo.m_DescriptorHeap != nullptr)
		{
			m_CPUHandleInfo.m_DescriptorHeap->ReleaseReourceView(m_CPUHandleInfo.m_UsedSlot);
		}

		m_Resource.Reset();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GPUResourceBase::GetCPUDescriptorHandle()
	{
		return m_CPUHandleInfo.m_DescriptorHeap->GetCPUDescriptorHandle(m_CPUHandleInfo.m_UsedSlot);
	}

	void GPUResourceBase::FillTransitionBarrierToNewState(D3D12_RESOURCE_BARRIER& Barrier, D3D12_RESOURCE_STATES NewState)
	{
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Resource.Get();
		Barrier.Transition.StateBefore = m_CurrentState;
		Barrier.Transition.StateAfter = NewState;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_PreviousState = m_CurrentState;
		m_CurrentState = NewState;
	}

	void GPUResourceBase::FillTransitionBarrierToPreviousState(D3D12_RESOURCE_BARRIER& Barrier)
	{
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Resource.Get();
		Barrier.Transition.StateBefore = m_CurrentState;
		Barrier.Transition.StateAfter = m_PreviousState;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		D3D12_RESOURCE_STATES TempState = m_PreviousState;
		m_PreviousState = m_CurrentState;
		m_CurrentState = TempState;
	}

	void GPUResourceBase::TransitToNewState(ID3D12GraphicsCommandList6* CommandList, D3D12_RESOURCE_STATES NewState)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier {};
		FillTransitionBarrierToNewState(ResourceBarrier, NewState);

		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}

	void GPUResourceBase::TransitToPreviousState(ID3D12GraphicsCommandList6* CommandList)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier {};
		FillTransitionBarrierToPreviousState(ResourceBarrier);

		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}

	void GPUResourceBase::SetCPUHandleInfo(DescriptorHeap* InDescriptorHeap, uint16_t InUsedSlot)
	{
		m_CPUHandleInfo.m_DescriptorHeap = InDescriptorHeap;
		m_CPUHandleInfo.m_UsedSlot = InUsedSlot;
	}

	void GPUResourceBase::SetName(std::wstring NewName)
	{
		m_Resource->SetName(NewName.c_str());
	}

}