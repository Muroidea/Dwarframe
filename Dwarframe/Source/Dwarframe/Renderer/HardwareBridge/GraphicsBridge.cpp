#include "pch.h"
#include "GraphicsBridge.h"

#include "Dwarframe/Renderer/DXContext.h"

namespace Dwarframe {

	void GraphicsBridge::Initialize()
	{
        UINT DXGIFactoryFlags = 0;
    #ifdef _DEBUG 
        DXGIFactoryFlags= DXGI_CREATE_FACTORY_DEBUG;
    #endif

        ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&m_DXGIFactory)));
        
        D3D_FEATURE_LEVEL m_DesiredFeatureLevel = D3D_FEATURE_LEVEL_12_1;

        ComPtr<IDXGIAdapter1> Adapter;

        size_t MaxDedicatedVideoMemory = 0;

        for (UINT AdapterIndex = 0; ; AdapterIndex++)
        {
            HRESULT hRes = m_DXGIFactory->EnumAdapterByGpuPreference(AdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter));
            if (hRes == DXGI_ERROR_NOT_FOUND)
            {
                break;
            }

            DXGI_ADAPTER_DESC1 AdapterDesc;
            Adapter->GetDesc1(&AdapterDesc);

            if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) { continue; }

            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), m_DesiredFeatureLevel, __uuidof(ID3D12Device), nullptr)) &&
                AdapterDesc.DedicatedVideoMemory > MaxDedicatedVideoMemory)
            {
                MaxDedicatedVideoMemory = AdapterDesc.DedicatedVideoMemory;
                ThrowIfFailed(Adapter.As(&m_DXGIChosenAdapter));
            }
        }
        //Adapter.Reset();

        // Create device
        ThrowIfFailed(D3D12CreateDevice(m_DXGIChosenAdapter.Get(), m_DesiredFeatureLevel, IID_PPV_ARGS(&m_D3D12Device)));
        
        // Check interesting features support
        D3D12_FEATURE_DATA_D3D12_OPTIONS12 Features12 = {};
        bool EnhancedBarriersSupport = false; // 
        m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &Features12, sizeof(Features12));
        EnhancedBarriersSupport = Features12.EnhancedBarriersSupported;

        D3D12_FEATURE_DATA_D3D12_OPTIONS7 Features7 = {};
        bool MeshShadersSupport = false; // 
        ThrowIfFailed(m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &Features7, sizeof(Features7)));
        MeshShadersSupport = Features7.MeshShaderTier;
        
        bool DesiredModelShaderSupported = false;
        for (uint32_t ID = D3D_HIGHEST_SHADER_MODEL; ID > D3D_SHADER_MODEL_6_0; ID--)
        {
            D3D12_FEATURE_DATA_SHADER_MODEL m_FeatureShaderModel = { static_cast<D3D_SHADER_MODEL>(ID) };

            if (SUCCEEDED(m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &m_FeatureShaderModel, sizeof(m_FeatureShaderModel))))
            {
                m_ShaderModel = m_FeatureShaderModel.HighestShaderModel;
                DesiredModelShaderSupported = true;
                break;
            }
        }

        if (!DesiredModelShaderSupported)
        {
            printf("ERROR: Shader Model 6.5 is not supported\n");
            throw std::exception("Shader Model 6.5 is not supported");
        }
        else
        {
            m_ShaderModelShortcut = ShaderModelToShortcut(m_ShaderModel);
            printf("Highest shader model supported: %s\n", m_ShaderModelShortcut);
        }

        /*
        if (FAILED(m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &ShaderModel, sizeof(ShaderModel)))
                || (ShaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5))
        {
            OutputDebugStringA("ERROR: Shader Model 6.5 is not supported\n");
            throw std::exception("Shader Model 6.5 is not supported");
        }
        */

        D3D12_FEATURE_DATA_D3D12_OPTIONS7 Features = {};
        if (FAILED(m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &Features, sizeof(Features)))
            || (Features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED))
        {
            OutputDebugStringA("ERROR: Mesh Shaders aren't supported!\n");
            throw std::exception("Mesh Shaders aren't supported!");
        }

        // Create command queue
        D3D12_COMMAND_QUEUE_DESC CommandQueueDesc {};
        CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        CommandQueueDesc.NodeMask = 0;

        ThrowIfFailed(m_D3D12Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_D3D12CommandQueue)));
	}

	void GraphicsBridge::Shutdown()
	{
        m_DepthStencilBuffer.Reset();
		m_DSVHeap.Reset();

        ReleaseFrameBuffers();
        m_D3D12RTVDescriptorHeap.Reset();

        m_DXGISwapChain.Reset();
        m_D3D12CommandQueue.Reset();
		m_D3D12Device.Reset();
        m_DXGIChosenAdapter.Reset();
        m_DXGIFactory.Reset();
	}

    void GraphicsBridge::InitializeSwapChain(HWND WindowHandle, unsigned int Width, unsigned int Height)
    {
        DXGI_SWAP_CHAIN_DESC1 SwapChainDesc {};
        SwapChainDesc.Width = Width;
        SwapChainDesc.Height = Height;
        SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SwapChainDesc.Stereo = false;
        SwapChainDesc.SampleDesc.Count = 1; // Multisampling desc
        SwapChainDesc.SampleDesc.Quality = 0;
        SwapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SwapChainDesc.BufferCount = Window::GetNumOfFrames();
        SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapChainFullscreenDesc {};
        SwapChainFullscreenDesc.Windowed = true;
        // We can ignore other values as long as it's not set to false

        ComPtr<IDXGISwapChain1> SwapChain1;
        ThrowIfFailed(m_DXGIFactory->CreateSwapChainForHwnd(m_D3D12CommandQueue.Get(), WindowHandle, &SwapChainDesc, &SwapChainFullscreenDesc, nullptr, &SwapChain1));
        ThrowIfFailed(SwapChain1.As(&m_DXGISwapChain));

        SwapChain1.Reset();

        //****** Creation of frames render targets ******
        D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc {};
        RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        RTVHeapDesc.NumDescriptors = Window::GetNumOfFrames();
        RTVHeapDesc.NodeMask = 0;

        ThrowIfFailed(m_D3D12Device->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&m_D3D12RTVDescriptorHeap)));
    
        //*** Create handles to view ***
        D3D12_CPU_DESCRIPTOR_HANDLE CPUHandleStart = m_D3D12RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        unsigned int IncrementSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (size_t i = 0; i < Window::GetNumOfFrames(); i++)
        {
            m_RTVHandles[i] = CPUHandleStart;
            m_RTVHandles[i].ptr += i * IncrementSize;
        }

        GetFrameBuffers();

        //****** Creation of depth stencil ******
        D3D12_DESCRIPTOR_HEAP_DESC DSVHeapDesc {};
        DSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        DSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DSVHeapDesc.NumDescriptors = 1;
        DSVHeapDesc.NodeMask = 0;

        ThrowIfFailed(m_D3D12Device->CreateDescriptorHeap(&DSVHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

        InitializeDepthStencilBuffer(Width, Height);
    }

    void GraphicsBridge::ResizeSwapChainsBuffers(unsigned int Width, unsigned int Height)
    {
        DXContext::Get().Flush();
        
        //****** Resize frames render targets ******
        ReleaseFrameBuffers();
        ThrowIfFailed(m_DXGISwapChain->ResizeBuffers(Window::GetNumOfFrames(), Width, Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));
        GetFrameBuffers();

        //****** Resize depth stencil ******
        m_DepthStencilBuffer.Reset();
        InitializeDepthStencilBuffer(Width, Height);
    }

    void GraphicsBridge::InitializeDepthStencilBuffer(unsigned int Width, unsigned int Height)
    {
        D3D12_HEAP_PROPERTIES HeapProperties {};
        HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        HeapProperties.CreationNodeMask = 0;
        HeapProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC ResourceDesc {};
        ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		ResourceDesc.Width = Width;
		ResourceDesc.Height = Height;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 0;
		ResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE OptimizedClearValue {};
        OptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        OptimizedClearValue.DepthStencil.Depth = 1.0f;
        OptimizedClearValue.DepthStencil.Stencil = 0;

        ThrowIfFailed(m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &OptimizedClearValue, IID_PPV_ARGS(&m_DepthStencilBuffer)));

        D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc {};
        DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
        DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        DSVDesc.Texture2D.MipSlice = 0;
        DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

        m_D3D12Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &DSVDesc, m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
        m_DSVHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
    }

    void GraphicsBridge::GetFrameBuffers()
    {
        for (size_t i = 0; i < Window::GetNumOfFrames(); i++)
        {
            ThrowIfFailed(m_DXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&m_FrameBuffers[i])));
            std::wstring BufferName = L"FrameBuffer" + std::to_wstring(i);
            m_FrameBuffers[i]->SetName(BufferName.c_str());
            
            D3D12_RENDER_TARGET_VIEW_DESC RTVDesc {};
            RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            RTVDesc.Texture2D.MipSlice = 0;
            RTVDesc.Texture2D.PlaneSlice = 0;

            m_D3D12Device->CreateRenderTargetView(m_FrameBuffers[i].Get(), &RTVDesc, m_RTVHandles[i]);
        }
    }

    void GraphicsBridge::ReleaseFrameBuffers()
    {
        for (uint32_t FrameID = 0; FrameID < Window::GetNumOfFrames(); FrameID++)
        {
            m_FrameBuffers[FrameID].Reset();
        }
    }

    void GraphicsBridge::BeginFrame(ID3D12GraphicsCommandList6* CommandList)
    {
        //****** Clear render target ******
        m_CurrentFrameBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

        D3D12_RESOURCE_BARRIER ResourceBarrier {};
        ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        ResourceBarrier.Transition.pResource = m_FrameBuffers[m_CurrentFrameBufferIndex].Get();
        ResourceBarrier.Transition.Subresource = 0;
        ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        CommandList->ResourceBarrier(1, &ResourceBarrier);
        
        const float ClearColor0[] = { 0.33f, 0.33f, 0.33f, 1.0f };
        CommandList->ClearRenderTargetView(m_RTVHandles[m_CurrentFrameBufferIndex], ClearColor0, 0, nullptr);
        CommandList->ClearDepthStencilView(m_DSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        /*
        // We can specify different rects to clear partially.
        const float ClearColor1[] = { 0.0f, 1.0f, 0.0f, 1.0f };
        D3D12_RECT Rects[2];
        Rects[0].left = 0;
        Rects[0].right = 200;
        Rects[0].top = 0;
        Rects[0].bottom = 200;

        Rects[1].left = 200;
        Rects[1].right = 400;
        Rects[1].top = 200;
        Rects[1].bottom = 400;

        CommandList->ClearRenderTargetView(m_RTVHandles[m_CurrentFrameBufferIndex], ClearColor1, 2, Rects);
        
        const float ClearColor2[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        Rects[0].left = 0;
        Rects[0].right = 200;
        Rects[0].top = 200;
        Rects[0].bottom = 400;

        Rects[1].left = 200;
        Rects[1].right = 400;
        Rects[1].top = 0;
        Rects[1].bottom = 200;

        CommandList->ClearRenderTargetView(m_RTVHandles[m_CurrentFrameBufferIndex], ClearColor2, 2, Rects);
        */
        
        CommandList->OMSetRenderTargets(1, &m_RTVHandles[m_CurrentFrameBufferIndex], false, &m_DSVHandle);
    }

    void GraphicsBridge::EndFrame(ID3D12GraphicsCommandList6* CommandList)
    {
        D3D12_RESOURCE_BARRIER ResourceBarrier {};
        ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        ResourceBarrier.Transition.pResource = m_FrameBuffers[m_CurrentFrameBufferIndex].Get();
        ResourceBarrier.Transition.Subresource = 0;
        ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        CommandList->ResourceBarrier(1, &ResourceBarrier);
    }
}