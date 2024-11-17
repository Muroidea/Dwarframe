#include "pch.h"
#include "BaseRenderPass.h"

#include "Dwarframe/Window.h"
#include "Dwarframe/Renderer/DXContext.h"
#include "Dwarframe/Renderer/GPUResources/Buffer.h"
#include "Dwarframe/Renderer/GPUResources/UploadBuffer.h"
#include "Dwarframe/Renderer/RenderPasses/Renderable.h"
#include "Dwarframe/Resources/Material.h"

#include "Dwarframe/Editor/Editor.h"
#include "ImGUI/imgui.h"

namespace Dwarframe {

	BaseRenderPass::BaseRenderPass(std::string Name)
		: RenderPass(Name)
	{
		ID3D12Device8* DXDevice = DXContext::Device();
		m_UploadBuffer = new UploadBuffer(DXDevice, GetSizeAligned(m_UploadBufferSize, (uint32)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), L"BaseRenderPass_UploadBuffer");
		m_MeshConstantsBuffer = new Buffer(DXDevice, GetSizeBufferAligned(sizeof(MeshConstants)), L"BaseRenderPass_ConstantBuffer", D3D12_RESOURCE_STATE_COMMON);
		AdjustBarriers(4);

		ImGUIEditor::Get().RegisterEditorExtender(this);
	}

	BaseRenderPass::~BaseRenderPass()
	{
		if (m_MeshConstantsBuffer)
		{
			delete m_MeshConstantsBuffer;
			m_MeshConstantsBuffer = nullptr;
		}

		if (m_UploadBuffer)
		{
			delete m_UploadBuffer;
			m_UploadBuffer = nullptr;
		}
	}

	void BaseRenderPass::AddRenderable(Renderable* InRenderable)
	{
		if (!InRenderable->GetMaterial() || !InRenderable->GetMesh())
		{
			return;
		}

		if (InRenderable->GetMaterial()->GetRenderingPipelineType() != MeshShaderPipeline)
		{
			// TODO: Logging
			return;
		}

		// Remove renderable from this entity if exists and all associated data
		for (uint32 i = 0; i < m_Renderables.size(); i++)
		{
			if (m_Renderables[i]->GetRelatedEntity() == InRenderable->GetRelatedEntity())
			{
				m_Renderables.erase(m_Renderables.begin() + i);

				m_PipelineStates.erase(m_PipelineStates.begin() + i);
				m_MeshVerticesBuffer.erase(m_MeshVerticesBuffer.begin() + i);
				m_MeshletTrianglesBuffer.erase(m_MeshletTrianglesBuffer.begin() + i);
				m_MeshletVertexIndicesBuffer.erase(m_MeshletVertexIndicesBuffer.begin() + i);
				m_MeshletsBuffer.erase(m_MeshletsBuffer.begin() + i);

				break;
			}
		}

		RenderPass::AddRenderable(InRenderable);

		//if (!InRenderable->GetMesh()->IsLoadedGPU())
		{
			m_PrimitivesToUpload.push_back(InRenderable);
		}
	}

	void BaseRenderPass::Render()
	{
		ID3D12GraphicsCommandList6* CommandList = DXContext::CommandList();
		
        //****** Rasterizer ******
        D3D12_VIEWPORT Viewport {};
        Viewport.TopLeftX = 0.0f;
        Viewport.TopLeftY = 0.0f;
        Viewport.Width = Window::Get().GetWindowWidth();
        Viewport.Height = Window::Get().GetWindowHeight();
        Viewport.MinDepth = D3D12_MIN_DEPTH;
        Viewport.MaxDepth = D3D12_MAX_DEPTH;
        CommandList->RSSetViewports(1, &Viewport);

        D3D12_RECT ScissorRect {};
        ScissorRect.left = 0;
        ScissorRect.top = 0;
        ScissorRect.right = Window::Get().GetWindowWidth();
        ScissorRect.bottom = Window::Get().GetWindowHeight();
        CommandList->RSSetScissorRects(1, &ScissorRect);
		
		for (uint32 i = 0; i < m_Renderables.size(); i++)
		{
			CommandList->SetPipelineState(m_PipelineStates[i].GetPipelineState());
			CommandList->SetGraphicsRootSignature(m_Renderables[i]->GetMaterial()->GetRootSignature());
			
			CommandList->SetGraphicsRootConstantBufferView(0, m_MeshConstantsBuffer->GetGPUVirtualAddress());
			BindMeshletData(i);
			CommandList->DispatchMesh(m_Renderables[i]->GetMesh()->GetSubmesh(0).Meshlets.size(), 1, 1);
		}
	}

	void BaseRenderPass::Update()
	{
		ID3D12GraphicsCommandList6* CommandList = DXContext::CommandList();

		bool bAddedThisUpdate = false;
		for (Renderable* Primitive : m_PrimitivesToUpload)
		{
			if (Primitive->GetMaterial()->GetRootSignature())
			{
				MeshPipelineState NewPipelineState;
				NewPipelineState.CreatePipelineState(DXContext::Device(), Primitive->GetMaterial());

				m_PipelineStates.push_back(NewPipelineState);
			}
			else
			{
				break;
			}

			bAddedThisUpdate = true;
			Mesh* CurrentMesh = Primitive->GetMesh();
			for (uint8 i = 0; i < CurrentMesh->GetNumOfSubmeshes(); i++)
			{
				Submesh& CurrentSubmesh = CurrentMesh->GetSubmesh(i);

				SubmitMeshletDataToUpload(CurrentSubmesh);
				//m_UploadBuffer->FlushSubmittedData(DXContext::CommandList());
				//FillTransitionBarrierToNewStateMeshlets(CurrentSubmesh, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				//ResourceBarrier();
			}
		}

		m_PrimitivesToUpload.clear();


		// Update projection matrix.
		XMMATRIX ProjectionMatrix = XMMatrixIdentity();
		if (bEnableProjectionMatrix)
		{
			float32 AspectRatio = static_cast<float32>(Window::Get().GetWindowWidth()) / static_cast<float32>(Window::Get().GetWindowHeight());
			ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(FieldOfView), AspectRatio, 0.1f, 1000.0f);
		}

		//Update view matrix
		XMMATRIX ViewMatrix = XMMatrixIdentity();
		if (bEnableViewMatrix)
		{
			ViewMatrix = XMMatrixLookAtLH(EyePosition, TargetPoint, UpDirection);
		}

		// Update object constants
		XMMATRIX WorldMatrix;
		WorldMatrix = XMMatrixScalingFromVector(Scale);
		WorldMatrix *= XMMatrixRotationAxis(RotationAxis, XMConvertToRadians(RotationAngle));
		WorldMatrix *= XMMatrixTranslationFromVector(Translation);

		XMStoreFloat4x4(&m_MeshConstants.World, XMMatrixTranspose(WorldMatrix));
		XMStoreFloat4x4(&m_MeshConstants.WorldView, XMMatrixTranspose(WorldMatrix * ViewMatrix));
		XMStoreFloat4x4(&m_MeshConstants.WorldViewProj, XMMatrixTranspose(WorldMatrix * ViewMatrix * ProjectionMatrix));
		m_MeshConstants.DrawMeshlets = false;
		/*
		FillTransitionBarrierToNewState_UnSafe(m_MeshConstantsBuffer, D3D12_RESOURCE_STATE_COPY_DEST);
		ResourceBarrier();

		// This should be done elsewhere and update only when it's necessary
		m_UploadBuffer->SubmitDataToUpload(m_MeshConstantsBuffer, &m_MeshConstants, sizeof(MeshConstants));

		// All Barriers for uploaded meshes are filled during Update
		if (m_UploadBuffer->AnythingToUpload())
		{
			m_UploadBuffer->FlushSubmittedData(CommandList);
		}

		FillTransitionBarrierToNewState_UnSafe(m_MeshConstantsBuffer, D3D12_RESOURCE_STATE_COMMON);
		ResourceBarrier();
		*/
		
		//************************************
		//UPDATE:
		FillTransitionBarrierToNewState_UnSafe(m_MeshConstantsBuffer, D3D12_RESOURCE_STATE_COPY_DEST);

		// This should be done elsewhere and update only when it's necessary
		m_UploadBuffer->SubmitDataToUpload(m_MeshConstantsBuffer, &m_MeshConstants, sizeof(MeshConstants));
		/*
		if (!m_Renderables.empty() && !bAddedThisUpdate)
		{
			FillTransitionBarrierToNewState_UnSafe(m_MeshVerticesBuffer.back(), D3D12_RESOURCE_STATE_COPY_DEST);

			Submesh& CurrentSubmesh = m_Renderables[0]->GetMesh()->GetSubmesh(0);
			uint64 SizeInBytes = CurrentSubmesh.MeshVertices.size() * sizeof(float32);

			m_UploadBuffer->SubmitDataToUpload(m_MeshVerticesBuffer.back(), &CurrentSubmesh.MeshVertices[0], SizeInBytes);
		}*/

		ResourceBarrier();

		// All Barriers for uploaded meshes are filled during Update
		if (m_UploadBuffer->AnythingToUpload())
		{
			m_UploadBuffer->FlushSubmittedData(CommandList);
		}

		FillTransitionBarrierToNewState_UnSafe(m_MeshConstantsBuffer, D3D12_RESOURCE_STATE_COMMON);
		/*
		if (!m_Renderables.empty() && !bAddedThisUpdate)
		{
			FillTransitionBarrierToNewState_UnSafe(m_MeshVerticesBuffer.back(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
		else*/ if (!m_Renderables.empty() && bAddedThisUpdate)
		{
			Submesh& CurrentSubmesh = m_Renderables[0]->GetMesh()->GetSubmesh(0);
			FillTransitionBarrierToNewStateMeshlets(CurrentSubmesh, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}

		ResourceBarrier();
		// UPDATE END
		//************************************
	}

	void BaseRenderPass::SetUploadBufferSize(uint32 NewSize)
	{
		if (m_UploadBufferSize == NewSize)
		{
			return;
		}

		// TODO: Ensure the size is not too big. What it even means too big?
		m_UploadBufferSize = NewSize;
		RecreateUploadBuffer();
	}

	void BaseRenderPass::RecreateUploadBuffer()
	{
		m_UploadBuffer->FlushSubmittedData(DXContext::CommandList());
		delete m_UploadBuffer;
		m_UploadBuffer = new UploadBuffer(DXContext::Device(), GetSizeAligned(m_UploadBufferSize, (uint32)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), L"BaseRenderPass_UploadBuffer");
	}

	bool BaseRenderPass::Extends(std::string ElementName)
	{
		return ElementName == EditorNames::LeftWindow;
	}

	void BaseRenderPass::RenderOptions()
	{
		if (!ImGui::CollapsingHeader("Base Render Pass generals:"))
		{
			return;
		}

		float Max = std::numeric_limits<float>::max();
		/*
		ImGui::NewLine();

		ImGui::Text("Dupa:");
		ImGui::InputInt("NumOfMeshletsToRender", &NumOfMeshletsToRender);
		

		ImGui::NewLine();
		ImGui::Separator();*/
		ImGui::NewLine();

		ImGui::Text("Projection matrix config:");
		ImGui::PushID(0);
		ImGui::Checkbox("Enable:", &bEnableProjectionMatrix);
		ImGui::DragFloat("FieldOfView", &FieldOfView, 1.0f, 45.0f, 120.0f); 
		ImGui::PopID();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text("View matrix config:");
		ImGui::PushID(1);
		ImGui::Checkbox("Enable:", &bEnableViewMatrix);
		ImGui::PopID();
		ImGui::DragFloat3("EyePosition", (float*) & EyePosition, 0.1f, -Max, Max);
		ImGui::DragFloat3("TargetPoint", (float*) & TargetPoint, 0.1f, -Max, Max);
		ImGui::DragFloat3("UpDirection", (float*) & UpDirection, 1.0f, 0.0f, 1.0f);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text("Model matrix config:");
		ImGui::DragFloat3("Scale", (float*)&Scale, 0.05f, 0.0f, Max);
		ImGui::DragFloat3("RotationAxis", (float*)&RotationAxis, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("RotationAngle", &RotationAngle, 1.0f, -Max, Max);
		ImGui::DragFloat3("Translation", (float*)&Translation, 0.1f, -Max, Max);

		ImGui::NewLine();
	}

	void BaseRenderPass::AdjustBarriers(uint32 NewElements)
	{
		if (m_Barriers.size() < m_LastFreeBarrier + NewElements)
		{
			m_Barriers.resize(m_LastFreeBarrier + 4);
		}
	}

	void BaseRenderPass::FillTransitionBarrierToNewState_UnSafe(GPUResourceBase* Resource, D3D12_RESOURCE_STATES NewState)
	{
		Resource->FillTransitionBarrierToNewState(m_Barriers[m_LastFreeBarrier], NewState);
		m_LastFreeBarrier++;
	}

	void BaseRenderPass::ResourceBarrier()
	{
		DXContext::CommandList()->ResourceBarrier(m_LastFreeBarrier, m_Barriers.data());
		m_LastFreeBarrier = 0;
	}

	void BaseRenderPass::FillTransitionBarrierToNewStateMeshlets(const Submesh& InSubmesh, D3D12_RESOURCE_STATES NewState)
	{
		AdjustBarriers(4);
		m_MeshVerticesBuffer.back()->FillTransitionBarrierToNewState(m_Barriers[m_LastFreeBarrier], NewState);
		m_MeshletsBuffer.back()->FillTransitionBarrierToNewState(m_Barriers[m_LastFreeBarrier + 1], NewState);
		m_MeshletVertexIndicesBuffer.back()->FillTransitionBarrierToNewState(m_Barriers[m_LastFreeBarrier + 2], NewState);
		m_MeshletTrianglesBuffer.back()->FillTransitionBarrierToNewState(m_Barriers[m_LastFreeBarrier + 3], NewState);
		m_LastFreeBarrier += 4;
	}

	void BaseRenderPass::SubmitMeshletDataToUpload(Submesh& InSubmesh)
	{
		ID3D12Device8* Device = DXContext::Device();
		uint64 SizeInBytes;
		std::wstring DebugName;

		SizeInBytes = GetSizeBufferAligned(InSubmesh.NumOfVertices * InSubmesh.Attributes.GetVertexStrideInBytes());
		DebugName = L"MeshVerticesBuffer";
		m_MeshVerticesBuffer.push_back(new Buffer(Device, SizeInBytes, DebugName));
		m_UploadBuffer->SubmitDataToUpload(m_MeshVerticesBuffer.back(), &InSubmesh.MeshVertices[0], SizeInBytes);
		

		SizeInBytes = GetSizeBufferAligned(InSubmesh.Meshlets.size() * sizeof(Submesh::Meshlet));
		DebugName = L"MeshletsBuffer";
		m_MeshletsBuffer.push_back(new Buffer(Device, SizeInBytes, DebugName));
		m_UploadBuffer->SubmitDataToUpload(m_MeshletsBuffer.back(), reinterpret_cast<void*>(InSubmesh.Meshlets.data()), SizeInBytes);


		SizeInBytes = GetSizeBufferAligned(InSubmesh.MeshletVertexIndices.size() * sizeof(uint32));
		DebugName = L"MeshletVertexIndicesBuffer";
		m_MeshletVertexIndicesBuffer.push_back(new Buffer(Device, SizeInBytes, DebugName));
		m_UploadBuffer->SubmitDataToUpload(m_MeshletVertexIndicesBuffer.back(), reinterpret_cast<void*>(InSubmesh.MeshletVertexIndices.data()), SizeInBytes);


		SizeInBytes = GetSizeBufferAligned(InSubmesh.MeshletTriangles.size() * sizeof(Submesh::MeshletTriangle));
		DebugName = L"MeshletTrianglesBuffer";
		m_MeshletTrianglesBuffer.push_back(new Buffer(Device, SizeInBytes, DebugName));
		m_UploadBuffer->SubmitDataToUpload(m_MeshletTrianglesBuffer.back(), reinterpret_cast<void*>(InSubmesh.MeshletTriangles.data()), SizeInBytes);
	}

	void BaseRenderPass::BindMeshletData(uint32 BufferID)
	{
		ID3D12GraphicsCommandList6* CommandList = DXContext::CommandList();

		CommandList->SetGraphicsRootShaderResourceView(1, m_MeshVerticesBuffer[BufferID]->GetGPUVirtualAddress());
		CommandList->SetGraphicsRootShaderResourceView(2, m_MeshletsBuffer[BufferID]->GetGPUVirtualAddress());
		CommandList->SetGraphicsRootShaderResourceView(3, m_MeshletVertexIndicesBuffer[BufferID]->GetGPUVirtualAddress());
		CommandList->SetGraphicsRootShaderResourceView(4, m_MeshletTrianglesBuffer[BufferID]->GetGPUVirtualAddress());
	}

}