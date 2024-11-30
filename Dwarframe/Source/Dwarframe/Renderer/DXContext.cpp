#include "pch.h"
#include "DXContext.h"

#include "Dwarframe/Helpers/TextureLoader.h"
#include "Dwarframe/Core/Window.h"
#include "HardwareBridge/GraphicsBridge.h"

#include "Dwarframe/Helpers/Alignments.h"
#include "Dwarframe/Resources/Mesh.h"
#include "Dwarframe/Resources/Texture.h"
#include "Dwarframe/Renderer/GPUResources/Shader.h"
#include "Dwarframe/Renderer/GPUResources/UploadBuffer.h"
#include "Dwarframe/Renderer/GPUResources/Texture2D.h"
#include "Dwarframe/Renderer/GPUResources/Buffer.h"

namespace Dwarframe {

	void DXContext::Initialize()
	{
        m_D3D12Device = GraphicsBridge::Get().GetD3D12Device();
        m_D3D12CommandQueue = GraphicsBridge::Get().GetD3D12CommandQueue();
        
        ThrowIfFailed(m_D3D12Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_D3D12Fence)), "Creation of D3D12 fence failed.");

        m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
        assert(m_FenceEvent && "Failed to create fence event handle.");

        ThrowIfFailed(m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_D3D12CommandAllocator)), "Creation of D3D12 command allocator failed.");
        ThrowIfFailed(m_D3D12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_D3D12CommandList)), "Creation of D3D12 command list failed.");

        /*
        // Fun with memory allocation
        ComPtr<ID3D12Heap> m_D3D12Heap;
        D3D12_HEAP_DESC HeapDescriptor {};
        HeapDescriptor.SizeInBytes = 128 * 1024 * 1024; // 128 MB
        HeapDescriptor.Properties.

        m_D3D12Device->CreateHeap(&HeapDescriptor, IID_PPV_ARGS(&m_D3D12Heap));

        m_D3D12Heap.Reset();*/

	}

	void DXContext::Shutdown()
	{
        Flush();
        
        delete PixelShader;
        PixelShader = nullptr;

        delete VertexShader;
        VertexShader = nullptr;
        
        delete RootSignatureShader;
        RootSignatureShader = nullptr;
        
		delete m_UploadBuffer;
		delete m_CorrectionConstantBuffer;
		delete m_MeshConstantBuffer;
		//delete m_BrickTexture;

        m_ClassicRootSignature.Release();

        m_SRV_CBV_DescriptorHeap.Shutdown();

        m_D3D12CommandList.Reset();
        m_D3D12CommandAllocator.Reset();

        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
        }
        m_D3D12Fence.Reset();

        m_D3D12CommandQueue.Reset();
		m_D3D12Device.Reset();
	}

    void DXContext::Update(float32 DeltaTime)
    {
        //****** Output merger ******
        // Num of render targets can be diffrent with deffered rendering for example with GBuffer that consists of several Render targets
        //m_D3D12CommandList->OMSetRenderTargets(1, GraphicsBridge::Get().GetCurrentRenderTarget(), false, GraphicsBridge::Get().GetDepthStencil());

        //****** Rasterizer ******
        D3D12_VIEWPORT Viewport {};
        Viewport.TopLeftX = 0.0f;
        Viewport.TopLeftY = 0.0f;
        Viewport.Width = Window::Get().GetWindowWidth();
        Viewport.Height = Window::Get().GetWindowHeight();
        Viewport.MinDepth = D3D12_MIN_DEPTH;
        Viewport.MaxDepth = D3D12_MAX_DEPTH;
        //m_D3D12CommandList->RSSetViewports(1, &Viewport);

        D3D12_RECT ScissorRect {};
        ScissorRect.left = 0;
        ScissorRect.top = 0;
        ScissorRect.right = Window::Get().GetWindowWidth();
        ScissorRect.bottom = Window::Get().GetWindowHeight();
        //m_D3D12CommandList->RSSetScissorRects(1, &ScissorRect);

        //****** Update ******
		Correction CorrectionData {};
		CorrectionData.R = 0.25f;
		CorrectionData.G = 0.75f;
		CorrectionData.B = 0.95f;
        CorrectionData.AspectRatio = (float32)Window::Get().GetWindowHeight() / (float32)Window::Get().GetWindowWidth();
        /*
        //****** Root ******
        m_D3D12CommandList->SetGraphicsRoot32BitConstants(0, 4, &CorrectionData, 0);
        m_D3D12CommandList->SetGraphicsRootDescriptorTable(1, m_SRV_CBV_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        
        //****** Input assembler ******

        m_D3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        D3D12_VERTEX_BUFFER_VIEW VertexBufferViews [3];

        VertexBufferViews[0].BufferLocation = m_CubePositionBuffer->GetGPUVirtualAddress();
        VertexBufferViews[0].SizeInBytes = m_CubeMesh.GetPositionsSize();
        VertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
        
        VertexBufferViews[1].BufferLocation = m_CubeNormalBuffer->GetGPUVirtualAddress();
        VertexBufferViews[1].SizeInBytes = m_CubeMesh.GetNormalsSize();
        VertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
        
        VertexBufferViews[2].BufferLocation = m_CubeUVBuffer->GetGPUVirtualAddress();
        VertexBufferViews[2].SizeInBytes = m_CubeMesh.GetUVsSize();
        VertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
        m_D3D12CommandList->IASetVertexBuffers(0, 3, VertexBufferViews);

        
        D3D12_INDEX_BUFFER_VIEW IndexBufferView {};
        IndexBufferView.BufferLocation = m_CubeIndexBuffer->GetGPUVirtualAddress();
        IndexBufferView.SizeInBytes = m_CubeMesh.GetIndicesSize();
        IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        
        m_D3D12CommandList->IASetIndexBuffer(&IndexBufferView);
        */
        // Update the view matrix.
        const XMVECTOR EyePosition = XMVectorSet(0, 8, -10.0f, 1.0f);
        const XMVECTOR TargetPoint = XMVectorSet(0, 0, 0, 1.0f);
        const XMVECTOR UpDirection = XMVectorSet(0, 1.0f, 0, 0);
        m_ViewMatrix = XMMatrixLookAtLH(EyePosition, TargetPoint, UpDirection);

        // Update the projection matrix.
        float32 AspectRatio = static_cast<float32>(Window::Get().GetWindowWidth()) / static_cast<float32>(Window::Get().GetWindowHeight());
        m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), AspectRatio, 0.1f, 1000.0f);
        
        /*
        // Update model matrices.
        RotationAngle += DeltaTime * 90.0f;
        const XMVECTOR RotationAxis0 = XMVectorSet(1.0f, 1.0f, 0, 0);
        const XMVECTOR RotationAxis1 = XMVectorSet(0, 1.0f, 0, 0);
        const XMVECTOR RotationAxis2 = XMVectorSet(0, 0, 1.0f, 0);

        XMMATRIX Matrices[NumOfCubes];

        Matrices[0] = XMMatrixScaling(4.0f, 4.0f, 4.0f);
        Matrices[1] = XMMatrixScaling(2.5f, 2.5f, 2.5f);
        Matrices[2] = XMMatrixScaling(2.0f, 10.0f, 1.0f);
        
        Matrices[0] *= XMMatrixRotationAxis(RotationAxis0, XMConvertToRadians(RotationAngle));
        Matrices[1] *= XMMatrixRotationAxis(RotationAxis1, XMConvertToRadians(RotationAngle));
        Matrices[2] *= XMMatrixRotationAxis(RotationAxis2, XMConvertToRadians(RotationAngle));
        
        Matrices[0] *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        Matrices[1] *= XMMatrixTranslation(5.0f, 0.0f, 0.0f);
        Matrices[2] *= XMMatrixTranslation(-5.0f, 0.0f, 0.0f);

        for (uint32 i = 0; i < NumOfCubes; i++)
        {
            Matrices[i] = XMMatrixMultiply(Matrices[i], m_ViewMatrix);
            Matrices[i] = XMMatrixMultiply(Matrices[i], m_ProjectionMatrix);;
        }

        D3D12_RESOURCE_BARRIER CorrectionBarrier;
        m_CorrectionConstantBuffer->FillTransitionBarrierToNewState(CorrectionBarrier, D3D12_RESOURCE_STATE_COPY_DEST);
        m_D3D12CommandList->ResourceBarrier(1, &CorrectionBarrier);
        
        m_UploadBuffer->SubmitDataToUpload(m_CorrectionConstantBuffer, &Matrices, NumOfCubes * sizeof(XMMATRIX));
        m_UploadBuffer->FlushSubmittedData(m_D3D12CommandList.Get());

        m_CorrectionConstantBuffer->FillTransitionBarrierToNewState(CorrectionBarrier, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        m_D3D12CommandList->ResourceBarrier(1, &CorrectionBarrier);
        
        //****** Draw geometry ******
        m_D3D12CommandList->DrawIndexedInstanced(m_CubeMesh.Indices.size(), NumOfCubes, 0, 0, 0);*/
        /*
        // Update Buddha matrix.
        const XMVECTOR RotationAxis4 = XMVectorSet(0.0f, 1.0f, 0, 0);

        XMMATRIX BuddhaMatrix;
        BuddhaMatrix = XMMatrixScaling(0.8f, 0.8f, 0.8f);
        
        BuddhaMatrix *= XMMatrixRotationAxis(RotationAxis4, XMConvertToRadians(RotationAngle));
        
        BuddhaMatrix *= XMMatrixTranslation(0.0f, -4.0f, 0.0f);

        BuddhaMatrix = XMMatrixMultiply(BuddhaMatrix, m_ViewMatrix);
        BuddhaMatrix = XMMatrixMultiply(BuddhaMatrix, m_ProjectionMatrix);
        
        D3D12_RESOURCE_BARRIER CorrectionBarrier2;
        m_CorrectionConstantBuffer->FillTransitionBarrierToNewState(CorrectionBarrier2, D3D12_RESOURCE_STATE_COPY_DEST);
        m_D3D12CommandList->ResourceBarrier(1, &CorrectionBarrier2);
        
        m_UploadBuffer->SubmitDataToUpload(m_CorrectionConstantBuffer, &BuddhaMatrix, NumOfBuddhas * sizeof(XMMATRIX));
        m_UploadBuffer->FlushSubmittedData(m_D3D12CommandList.Get());

        m_CorrectionConstantBuffer->FillTransitionBarrierToNewState(CorrectionBarrier2, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        m_D3D12CommandList->ResourceBarrier(1, &CorrectionBarrier2);
        
        m_BuddhaMesh.DrawMeshes(m_D3D12CommandList.Get(), NumOfBuddhas, NumOfCubes);
        */
        /*
        // Update meshlet constants
        RotationAngle += DeltaTime;
        const XMVECTOR MeshletRotationAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        XMMATRIX MeshletMatrix;
        MeshletMatrix = XMMatrixScaling(3.0f, 3.0f, 3.0f);
        MeshletMatrix *= XMMatrixRotationAxis(MeshletRotationAxis, XMConvertToRadians(RotationAngle));
        MeshletMatrix *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        m_MeshConstants.World = MeshletMatrix;
        m_MeshConstants.WorldView = XMMatrixMultiply(MeshletMatrix, m_ViewMatrix);
        m_MeshConstants.WorldViewProj = XMMatrixMultiply(m_MeshConstants.WorldView, m_ProjectionMatrix);
        m_MeshConstants.DrawMeshlets = false;

        D3D12_RESOURCE_BARRIER MeshletBarrier {};
        m_MeshConstantBuffer->FillTransitionBarrierToNewState(MeshletBarrier, D3D12_RESOURCE_STATE_COPY_DEST);
        m_D3D12CommandList->ResourceBarrier(1, &MeshletBarrier);
        
        m_UploadBuffer->SubmitDataToUpload(m_MeshConstantBuffer, &m_MeshConstants, sizeof(MeshConstants));
        m_UploadBuffer->FlushSubmittedData(m_D3D12CommandList.Get());

        m_MeshConstantBuffer->FillTransitionBarrierToNewState(MeshletBarrier, D3D12_RESOURCE_STATE_COMMON);
        m_D3D12CommandList->ResourceBarrier(1, &MeshletBarrier);

        m_D3D12CommandList->SetPipelineState(m_MeshPipeline.GetPipelineState());
        m_D3D12CommandList->SetGraphicsRootSignature(m_BaseMeshMaterial.GetRootSignature());
        

        //m_D3D12CommandList->SetDescriptorHeaps(0, nullptr);

        m_D3D12CommandList->SetGraphicsRootConstantBufferView(0, m_MeshConstantBuffer->GetGPUVirtualAddress());
        //m_Model.BindMeshletData(m_D3D12CommandList.Get());

        //m_D3D12CommandList->DispatchMesh(m_BuddhaMesh.GetNumOfMeshlets(0), 1, 1);
        //m_D3D12CommandList->DispatchMesh(m_Model.GeNumOfMeshlets(), 1, 1);
        */
    }

    void DXContext::LoadData()
    {
		VertexLayout m_VertexLayout;
        m_VertexLayout.AddAttribute(EAttribute::Position);
        m_VertexLayout.AddAttribute(EAttribute::Normal);
        m_VertexLayout.AddAttribute(EAttribute::TexCoord_0);
        

        //****** Buddha mesh ******
        //std::filesystem::path BuddhaMeshPath { std::filesystem::current_path().string() + "Meshes\\Katana.fbx" };
        //m_BuddhaMesh.SetVertexLayout(&m_VertexLayout);
        //MeshLoader::Load(BuddhaMeshPath, m_BuddhaMesh);
        //m_BuddhaMesh.CreateGPUData(m_D3D12Device.Get());
        
        //m_Model.SetVertexLayout(&m_VertexLayout);
        //MeshLoader::Load("Katana.fbx", m_Model, m_D3D12Device.Get());

        //****** Texture + Constant buffer ******
        //std::filesystem::path TexturePath { Paths::GetTexturesDir().append("Bricks080B\\Bricks080B_2K_Color.png") };
        //Texture* NewTexture = new Texture(TexturePath.string(), "Bricks");
        //TextureLoader::Load(TexturePath, NewTexture);
        
        //uint64 ConstantBuffeSize = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		//m_CorrectionConstantBuffer = new Buffer(m_D3D12Device.Get(), GetSizeAligned((uint64)(NumOfInstances * sizeof(XMMATRIX)), ConstantBuffeSize), L"CorrectionConstantBuffer");
		//m_MeshConstantBuffer = new Buffer(m_D3D12Device.Get(), GetSizeAligned(sizeof(MeshConstants), ConstantBuffeSize), L"MatrixConstantBuffer");
		//m_BrickTexture = new Texture2D(m_D3D12Device.Get(), m_TextureData, L"BrickTexture");

        //****** Upload buffer ******
        //uint64 UploadBufferSize = m_TextureData.SizeInBytes + 2 * ConstantBuffeSize + m_CubeMesh.GetSizeInBytes() + m_BuddhaMesh.GetSizeInBytes() + m_BuddhaMesh.GetMeshletDataSizeInBytes(0);
        //uint64 UploadBufferSize = m_TextureData.SizeInBytes + m_BuddhaMesh.GetMeshletDataSizeInBytes(0);
        //uint64 UploadBufferSize = m_Model.GetMeshletsDataSize() + m_Model.GetTexturesDataSize();
		//m_UploadBuffer = new UploadBuffer(m_D3D12Device.Get(), ConstantBuffeSize, L"UploadBuffer");
        
        
        //m_UploadBuffer->SubmitTextureToUpload(m_BrickTexture, m_TextureData);
        /*
        m_UploadBuffer->SubmitDataToUpload(m_CubePositionBuffer, m_CubeMesh.GetPositionsData(), m_CubeMesh.GetPositionsSize());
        m_UploadBuffer->SubmitDataToUpload(m_CubeNormalBuffer, m_CubeMesh.GetNormalsData(), m_CubeMesh.GetNormalsSize());
        m_UploadBuffer->SubmitDataToUpload(m_CubeUVBuffer, m_CubeMesh.GetUVsData(), m_CubeMesh.GetUVsSize());
        m_UploadBuffer->SubmitDataToUpload(m_CubeIndexBuffer, m_CubeMesh.GetIndicesData(), m_CubeMesh.GetIndicesSize());

        m_BuddhaMesh.SubmitDataToUpload(m_UploadBuffer);
        */
        
        //ResetCommandList();

		//m_BuddhaMesh.SubmitMeshletDataToUpload(m_D3D12Device.Get(), m_UploadBuffer);
        //m_Model.SubmitMeshletDataToUpload(m_D3D12Device.Get(), m_UploadBuffer);
        //m_Model.SubmitTextureDataToUpload(m_UploadBuffer);
        //m_UploadBuffer->FlushSubmittedData(m_D3D12CommandList.Get());

        //uint32 NeededBarriers = 4 /*Cube buffers*/ + 1 /*Texture*/ + 1 /*Constant Correction buffer*/ + 1 /*Meshlet constant buffer*/ + m_BuddhaMesh.GetNumOfNeededBarriers() + 4 /* Meshlets buffers */;
        //m_BrickTexture->FillTransitionBarrierToNewState(Barriers[0], D3D12_RESOURCE_STATE_COMMON);
        //m_CorrectionConstantBuffer->FillTransitionBarrierToNewState(Barriers[0], D3D12_RESOURCE_STATE_COMMON);
        //D3D12_RESOURCE_BARRIER ConstantBarrier;
        //m_MeshConstantBuffer->FillTransitionBarrierToNewState(ConstantBarrier, D3D12_RESOURCE_STATE_COMMON);
        //m_D3D12CommandList->ResourceBarrier(1, &ConstantBarrier);
        /*
        m_CubePositionBuffer->FillTransitionBarrierToNewState(Barriers[3], D3D12_RESOURCE_STATE_COMMON);
        m_CubeNormalBuffer->FillTransitionBarrierToNewState(Barriers[4], D3D12_RESOURCE_STATE_COMMON);
        m_CubeUVBuffer->FillTransitionBarrierToNewState(Barriers[5], D3D12_RESOURCE_STATE_COMMON);
        m_CubeIndexBuffer->FillTransitionBarrierToNewState(Barriers[6], D3D12_RESOURCE_STATE_COMMON);
        */
       // m_BuddhaMesh.FillTransitionBarrierToNewStateMeshletsData(&Barriers[1], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        //m_BuddhaMesh.FillTransitionBarrierToNewState(&Barriers[11], D3D12_RESOURCE_STATE_COMMON);
        //constexpr uint32 NeededBarriers = 6;
        //D3D12_RESOURCE_BARRIER Barriers[NeededBarriers];
        //m_Model.FillTransitionBarrierToNewStateMeshlets(&Barriers[0], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        //m_Model.FillTransitionBarrierToNewStateTextures(&Barriers[4], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        //m_D3D12CommandList->ResourceBarrier(NeededBarriers, Barriers);

        //DispatchCommandQueue();

        //****** Load Shaders ******
        /*
        VertexShader = new Shader("VertexShader.cso");
        PixelShader = new Shader("PixelShader.cso");

        m_BaseMeshMaterial.LoadMeshShader("MeshShader.cso");
        m_BaseMeshMaterial.LoadPixelShader("MeshPixelShader.cso");

		RootSignature& MeshRootSignature = m_BaseMeshMaterial.GetRootSignatureRef();
		MeshRootSignature.AddCBVParameter(0);
		MeshRootSignature.AddSRVParameter(0, D3D12_SHADER_VISIBILITY_MESH);
		MeshRootSignature.AddSRVParameter(1, D3D12_SHADER_VISIBILITY_MESH);
		MeshRootSignature.AddSRVParameter(2, D3D12_SHADER_VISIBILITY_MESH);
		MeshRootSignature.AddSRVParameter(3, D3D12_SHADER_VISIBILITY_MESH);

		MeshRootSignature.BeginDescriptorTableParameter(D3D12_SHADER_VISIBILITY_PIXEL); // Textures
		MeshRootSignature.AddSRVRange(1, 4);
		MeshRootSignature.AddSRVRange(1, 5);
		MeshRootSignature.EndDescriptorTableParameter();

		MeshRootSignature.AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL);
		MeshRootSignature.EndRootSignature(m_D3D12Device.Get(), D3D12_ROOT_SIGNATURE_FLAG_NONE);

        m_MeshPipeline.CreatePipelineState(m_D3D12Device.Get(), &m_BaseMeshMaterial);
        */
        //****** Root Signature ******
        // From shader
        //RootSignatureShader = new Shader("RootSignature.cso");
        //ThrowIfFailed(m_D3D12Device->CreateRootSignature(0, RootSignatureShader->GetShaderBinary(), RootSignatureShader->GetShaderSize(), IID_PPV_ARGS(&m_RootSignature)), "Creation of Root Signature failed.");


        /*
        m_ClassicRootSignature.AddConstantParameter(4, 0);
        
        m_ClassicRootSignature.BeginDescriptorTableParameter(D3D12_SHADER_VISIBILITY_ALL);
        //m_RootSignatureTest.AddCBVRange(1, 1);
        m_ClassicRootSignature.AddCBVRange(1, 1, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
        m_ClassicRootSignature.AddSRVRange(1, 0);
        m_ClassicRootSignature.EndDescriptorTableParameter();

        m_ClassicRootSignature.AddStaticSampler();
        m_ClassicRootSignature.EndRootSignature(m_D3D12Device.Get(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        */

        //m_SRV_CBV_DescriptorHeap.Initialize(m_D3D12Device.Get(), 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        //m_SRV_CBV_DescriptorHeap.AddConstantBufferView(m_D3D12Device.Get(), m_MeshConstantBuffer);
		//D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = m_SRV_CBV_DescriptorHeap.GetCPUDescriptorHandleForHeapStart();
		//uint32 IncrementSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        /*
		{
			//****** Correction CBV creation ******
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc{};
			CBVDesc.BufferLocation = m_CorrectionConstantBuffer->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = GetSizeAligned((uint64)(NumOfInstances * sizeof(XMMATRIX)), (uint64)256);

			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CPUDescriptorHandle);
        }

        {
            //****** Texture SRV ******
			CPUDescriptorHandle.ptr += IncrementSize;

            D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc {};
            SRVDesc.Format = m_TextureData.PixelFormat;
            SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            SRVDesc.Texture2D.MipLevels = 1;
            SRVDesc.Texture2D.MostDetailedMip = 0;
            SRVDesc.Texture2D.PlaneSlice = 0;
            SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        
            m_D3D12Device->CreateShaderResourceView(m_BrickTexture->GetResource(), &SRVDesc, CPUDescriptorHandle);
        }*/
        /*
        {
			//****** Meshlet CBV creation ******
            //CPUDescriptorHandle.ptr += IncrementSize;

			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc{};
            CBVDesc.BufferLocation = m_MeshConstantBuffer->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = GetSizeAligned(sizeof(MeshConstants), (uint64)256);

			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CPUDescriptorHandle);
        }*/
    }

    uint64 DXContext::Signal()
    {
        uint64 FenceValue = ++m_FenceValue;
        ThrowIfFailed(m_D3D12CommandQueue->Signal(m_D3D12Fence.Get(), FenceValue), "Signaling failed.");
        return FenceValue;
    }

    bool DXContext::IsFenceComplete(uint64 FenceValue)
    {
        return m_D3D12Fence->GetCompletedValue() >= FenceValue;
    }

    void DXContext::WaitForFenceValue(uint64 FenceValue)
    {
        ThrowIfFailed(m_D3D12Fence->SetEventOnCompletion(FenceValue, m_FenceEvent), "Setting event on completion failed.");
        WaitForSingleObjectEx(m_FenceEvent, INFINITE, false);
    }

    void DXContext::Flush()
    {
        WaitForFenceValue(Signal());
    }
}