#include "pch.h"
#include "DXDebugLayer.h"

namespace Dwarframe {

	void DXDebugLayer::Initialize()
    {
    #ifdef _DEBUG

        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12DebugInterface)), "Enabling D3D12 debug interface failed.");
        if (m_D3D12DebugInterface)
        {
            m_D3D12DebugInterface->EnableDebugLayer();
            m_D3D12DebugInterface->SetEnableGPUBasedValidation(true);
            m_D3D12DebugInterface->SetEnableSynchronizedCommandQueueValidation(true);
        }

        ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_DXGIDebugInterface)), "Enabling DXGI debug interface failed.");
        if (m_DXGIDebugInterface)
        {
            m_DXGIDebugInterface->EnableLeakTrackingForThread();
        }

    #endif
    }

    void DXDebugLayer::Shutdown()
    {
    #ifdef _DEBUG

        if (m_DXGIDebugInterface)
        {
            OutputDebugStringW(L"\nDXGI Reports living device objects:\n");
            m_DXGIDebugInterface->ReportLiveObjects(
                DXGI_DEBUG_ALL,
                DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
            );
            OutputDebugStringW(L"End of report.\n\n");
        }

        m_D3D12DebugInterface.Reset();
	    m_DXGIDebugInterface.Reset();

    #endif
    }

}

