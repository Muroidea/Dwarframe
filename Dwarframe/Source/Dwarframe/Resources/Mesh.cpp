#include "pch.h"
#include "Mesh.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

/*
	void Mesh::FillTransitionBarrierToNewState(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState)
	{
		m_IndicesBuffer->FillTransitionBarrierToNewState(*Barriers, NewState);

		uint32_t BarrierIndex = 1;
		for (uint32_t SubmeshID = 0; SubmeshID < m_Submeshes.size(); SubmeshID++)
		{
			uint32_t BufferIndex = 0;
			if (HasAttribute(EAttribute::Position))
			{
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->FillTransitionBarrierToNewState(*(Barriers + BarrierIndex), NewState);
				BufferIndex++;
				BarrierIndex++;
			}
            
			if (HasAttribute(EAttribute::Normal))
			{
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->FillTransitionBarrierToNewState(*(Barriers + BarrierIndex), NewState);
				BufferIndex++;
				BarrierIndex++;
			}
            
			if (HasAttribute(EAttribute::TexCoord_0))
			{
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->FillTransitionBarrierToNewState(*(Barriers + BarrierIndex), NewState);
				BufferIndex++;
				BarrierIndex++;
			}
            
			if (HasAttribute(EAttribute::Tangent))
			{
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->FillTransitionBarrierToNewState(*(Barriers + BarrierIndex), NewState);
				BufferIndex++;
				BarrierIndex++;
			}
            
			if (HasAttribute(EAttribute::Bitangent))
			{
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->FillTransitionBarrierToNewState(*(Barriers + BarrierIndex), NewState);
				BufferIndex++;
				BarrierIndex++;
			}
		}
	}

	void Mesh::SubmitDataToUpload(UploadBuffer* InUploadBuffer)
	{
		InUploadBuffer->SubmitDataToUpload(m_IndicesBuffer, m_Indices.data(), GetIndicesSizeInBytes());

		for (uint32_t SubmeshID = 0; SubmeshID < m_Submeshes.size(); SubmeshID++)
		{
			uint32_t BufferIndex = 0;
			if (HasAttribute(EAttribute::Position))
			{
				InUploadBuffer->SubmitDataToUpload(m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex], m_Submeshes[SubmeshID].m_Positions.data(), GetPositionsSizeInBytes(SubmeshID));
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Normal))
			{
				InUploadBuffer->SubmitDataToUpload(m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex], m_Submeshes[SubmeshID].m_Normals.data(), GetNormalsSizeInBytes(SubmeshID));
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::TexCoord))
			{
				InUploadBuffer->SubmitDataToUpload(m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex], m_Submeshes[SubmeshID].m_UVs.data(), GetUVsSizeInBytes(SubmeshID));
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Tangent))
			{
				InUploadBuffer->SubmitDataToUpload(m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex], m_Submeshes[SubmeshID].m_Tangents.data(), GetTangentsSizeInBytes(SubmeshID));
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Bitangent))
			{
				InUploadBuffer->SubmitDataToUpload(m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex], m_Submeshes[SubmeshID].m_Bitangents.data(), GetBitangentsSizeInBytes(SubmeshID));
				BufferIndex++;
			}
		}
	}

	void Mesh::CreateGPUData(ID3D12Device8* Device)
	{
		m_IndicesBuffer = new Buffer(Device, GetIndicesSizeInBytes(), m_Name + L"_Indices");

		for (uint32_t SubmeshID = 0; SubmeshID < m_Submeshes.size(); SubmeshID++)
		{
			m_Submeshes[SubmeshID].m_AttributeBuffers.resize(m_VertexLayout->GetNumOfAttributes());

			size_t SizeInBytes;
			std::string DebugName;
			uint32_t BufferIndex = 0;
			if (HasAttribute(EAttribute::Position))
			{
				SizeInBytes = GetPositionsSizeInBytes(SubmeshID);
				DebugName = m_Name + L"_Positions";
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex] = new Buffer(Device, SizeInBytes, DebugName);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Normal))
			{
				SizeInBytes = GetNormalsSizeInBytes(SubmeshID);
				DebugName = m_Name + L"_Normals";
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex] = new Buffer(Device, SizeInBytes, DebugName);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::TexCoord))
			{
				SizeInBytes = GetUVsSizeInBytes(SubmeshID);
				DebugName = m_Name + L"_TexCoords";
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex] = new Buffer(Device, SizeInBytes, DebugName);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Tangent))
			{
				SizeInBytes = GetTangentsSizeInBytes(SubmeshID);
				DebugName = m_Name + L"_Tangents";
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex] = new Buffer(Device, SizeInBytes, DebugName);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Bitangent))
			{
				SizeInBytes = GetBitangentsSizeInBytes(SubmeshID);
				DebugName = m_Name + L"_Bitangents";
				m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex] = new Buffer(Device, SizeInBytes, DebugName);
				BufferIndex++;
			}
		}
	}

	void Mesh::FillTransitionBarrierToNewStateMeshletsData(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState)
	{
		Submesh& Submesh = m_Submeshes[0];
		Submesh.m_VerticesBuffer->FillTransitionBarrierToNewState(Barriers[0], NewState);
		Submesh.m_MeshletTrianglesBuffer->FillTransitionBarrierToNewState(Barriers[1], NewState);
		Submesh.m_VertexIndicesBuffer->FillTransitionBarrierToNewState(Barriers[2], NewState);
		Submesh.m_MeshletsBuffer->FillTransitionBarrierToNewState(Barriers[3], NewState);
	}

	void Mesh::SubmitMeshletDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer)
	{
		size_t SizeInBytes;
		std::string DebugName;
		Submesh& Submesh = m_Submeshes[0];
			
		SizeInBytes = GetVerticesDataSizeInBytes(0);
		DebugName = L"VerticesBuffer";
		Submesh.m_VerticesBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_VerticesBuffer, Submesh.Vertices.data(), SizeInBytes);
		
		SizeInBytes = GetMeshletTrianglesSizeInBytes(0);
		DebugName = L"MeshletTrianglesBuffer";
		Submesh.m_MeshletTrianglesBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_MeshletTrianglesBuffer, Submesh.m_MeshletTriangles.data(), SizeInBytes);

		SizeInBytes = GetUniqueIndicesSizeInBytes(0);
		DebugName = L"VertexIndicesBuffer";
		Submesh.m_VertexIndicesBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_VertexIndicesBuffer, Submesh.m_VertexIndices.data(), SizeInBytes);

		SizeInBytes = GetMeshletsSizeInBytes(0);
		DebugName = L"MeshletsBuffer";
		Submesh.m_MeshletsBuffer = new Buffer(Device, SizeInBytes, DebugName);
		InUploadBuffer->SubmitDataToUpload(Submesh.m_MeshletsBuffer, Submesh.m_Meshlets.data(), SizeInBytes);
	}

	void Mesh::BindMeshletData(ID3D12GraphicsCommandList6* CommandList)
	{
		Submesh& Submesh = m_Submeshes[0];
		CommandList->SetGraphicsRootShaderResourceView(1, Submesh.m_VerticesBuffer->GetGPUVirtualAddress());
		CommandList->SetGraphicsRootShaderResourceView(2, Submesh.m_MeshletTrianglesBuffer->GetGPUVirtualAddress());
		CommandList->SetGraphicsRootShaderResourceView(3, Submesh.m_VertexIndicesBuffer->GetGPUVirtualAddress());
		CommandList->SetGraphicsRootShaderResourceView(4, Submesh.m_MeshletsBuffer->GetGPUVirtualAddress());
	}

	void Mesh::DrawMeshes(ID3D12GraphicsCommandList6* CommandList, uint32_t NumOfInstances, uint32_t InstancesBase)
	{
		// TODO: This can be a member of mesh class
        D3D12_INDEX_BUFFER_VIEW IndexBufferView {};
        IndexBufferView.BufferLocation = m_IndicesBuffer->GetGPUVirtualAddress();
        IndexBufferView.SizeInBytes = GetIndicesSizeInBytes();
        IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        
        CommandList->IASetIndexBuffer(&IndexBufferView);

		// TODO: This can be a member of mesh class
		D3D12_VERTEX_BUFFER_VIEW* VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[m_VertexLayout->GetNumOfAttributes()];
		for (uint32_t SubmeshID = 0; SubmeshID < m_Submeshes.size(); SubmeshID++)
		{
			uint32_t BufferIndex = 0;
			if (HasAttribute(EAttribute::Position))
			{	
				VertexBufferViews[BufferIndex].BufferLocation = m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->GetGPUVirtualAddress();
				VertexBufferViews[BufferIndex].SizeInBytes = GetPositionsSizeInBytes(SubmeshID);
				VertexBufferViews[BufferIndex].StrideInBytes = sizeof(DirectX::XMFLOAT3);

				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Normal))
			{
				VertexBufferViews[BufferIndex].BufferLocation = m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->GetGPUVirtualAddress();
				VertexBufferViews[BufferIndex].SizeInBytes = GetNormalsSizeInBytes(SubmeshID);
				VertexBufferViews[BufferIndex].StrideInBytes = sizeof(DirectX::XMFLOAT3);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::TexCoord))
			{
				VertexBufferViews[BufferIndex].BufferLocation = m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->GetGPUVirtualAddress();
				VertexBufferViews[BufferIndex].SizeInBytes = GetUVsSizeInBytes(SubmeshID);
				VertexBufferViews[BufferIndex].StrideInBytes = sizeof(DirectX::XMFLOAT2);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Tangent))
			{
				VertexBufferViews[BufferIndex].BufferLocation = m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->GetGPUVirtualAddress();
				VertexBufferViews[BufferIndex].SizeInBytes = GetTangentsSizeInBytes(SubmeshID);
				VertexBufferViews[BufferIndex].StrideInBytes = sizeof(DirectX::XMFLOAT3);
				BufferIndex++;
			}
            
			if (HasAttribute(EAttribute::Bitangent))
			{
				VertexBufferViews[BufferIndex].BufferLocation = m_Submeshes[SubmeshID].m_AttributeBuffers[BufferIndex]->GetGPUVirtualAddress();
				VertexBufferViews[BufferIndex].SizeInBytes = GetBitangentsSizeInBytes(SubmeshID);
				VertexBufferViews[BufferIndex].StrideInBytes = sizeof(DirectX::XMFLOAT3);
				BufferIndex++;
			}
        
			CommandList->IASetVertexBuffers(0, m_VertexLayout->GetNumOfAttributes(), VertexBufferViews);
			CommandList->DrawIndexedInstanced(m_Submeshes[SubmeshID].m_IndicesCount, NumOfInstances, m_Submeshes[SubmeshID].m_IndicesOffset, 0, InstancesBase);
		}
        
	}

	void Mesh::ResizeSubmesh(Submesh& Mesh, uint32_t VerticesCount, uint32_t IndicesCount)
	{
		if (m_VertexLayout->HasAttribute(EAttribute::Position))
        {
            Mesh.m_Positions.resize(VerticesCount);
        }
            
        if (m_VertexLayout->HasAttribute(EAttribute::Normal))
        {
            Mesh.m_Normals.resize(VerticesCount);
        }
            
        if (m_VertexLayout->HasAttribute(EAttribute::TexCoord))
        {
            Mesh.m_UVs.resize(VerticesCount);
        }
            
        if (m_VertexLayout->HasAttribute(EAttribute::Tangent))
        {
            Mesh.m_Tangents.resize(VerticesCount);
        }
            
        if (m_VertexLayout->HasAttribute(EAttribute::Bitangent))
        {
            Mesh.m_Bitangents.resize(VerticesCount);
        }

        Mesh.m_IndicesOffset = m_Indices.size();
		Mesh.m_IndicesCount = IndicesCount;
        m_Indices.resize(m_Indices.size() + IndicesCount);
	}

	uint32_t Mesh::GetSizeInBytes() const
	{
		uint32_t SizeInBytes = 0;
		for (uint32_t SubmeshID = 0; SubmeshID < m_Submeshes.size(); SubmeshID++)
		{
			SizeInBytes += GetPositionsSizeInBytes(SubmeshID);
			SizeInBytes += GetNormalsSizeInBytes(SubmeshID);
			SizeInBytes += GetUVsSizeInBytes(SubmeshID);
			SizeInBytes += GetTangentsSizeInBytes(SubmeshID);
			SizeInBytes += GetBitangentsSizeInBytes(SubmeshID);
		}

		SizeInBytes += GetIndicesSizeInBytes();

		return SizeInBytes;
	}

	uint32_t Mesh::GetSizeInBytes(uint32_t SubmeshID) const
	{
		uint32_t SizeInBytes = 0;
		SizeInBytes += GetPositionsSizeInBytes(SubmeshID);
		SizeInBytes += GetNormalsSizeInBytes(SubmeshID);
		SizeInBytes += GetUVsSizeInBytes(SubmeshID);
		SizeInBytes += GetTangentsSizeInBytes(SubmeshID);
		SizeInBytes += GetBitangentsSizeInBytes(SubmeshID);
		SizeInBytes += m_Submeshes[SubmeshID].m_IndicesCount * sizeof(uint32_t);

		return SizeInBytes;
	}

	uint32_t Mesh::GetMeshletDataSizeInBytes(uint32_t SubmeshID) const
	{
		uint32_t SizeInBytes = 0;
		SizeInBytes += GetVerticesDataSizeInBytes(SubmeshID);
		SizeInBytes += GetMeshletTrianglesSizeInBytes(SubmeshID);
		SizeInBytes += GetUniqueIndicesSizeInBytes(SubmeshID);
		SizeInBytes += GetMeshletsSizeInBytes(SubmeshID);

		return SizeInBytes;
	}
	*/
	VertexLayout::~VertexLayout()
	{
		if (m_InputElementDesc)
		{
			delete m_InputElementDesc;
			m_InputElementDesc = nullptr;
		}
	}
	
	D3D12_INPUT_ELEMENT_DESC* VertexLayout::GetInputElementDesc()
	{
		if (m_InputElementDesc)
		{
			return m_InputElementDesc;
		}

		m_InputElementDesc = new D3D12_INPUT_ELEMENT_DESC[m_NumOfAttributes];
		
		uint32_t AttributeIndex = 0;
		if (HasAttribute(EAttribute::Position))
        {
			m_InputElementDesc[AttributeIndex] = { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
        }
            
        if (HasAttribute(EAttribute::Normal))
        {
			m_InputElementDesc[AttributeIndex] = { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
        }
            
        if (HasAttribute(EAttribute::TexCoord_0))
        {
			m_InputElementDesc[AttributeIndex] = { "UV0", 0, DXGI_FORMAT_R32G32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
        }

		if (HasAttribute(EAttribute::TexCoord_1))
		{
			m_InputElementDesc[AttributeIndex] = { "UV1", 0, DXGI_FORMAT_R32G32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
		}

		if (HasAttribute(EAttribute::TexCoord_2))
		{
			m_InputElementDesc[AttributeIndex] = { "UV2", 0, DXGI_FORMAT_R32G32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
		}

		if (HasAttribute(EAttribute::TexCoord_3))
		{
			m_InputElementDesc[AttributeIndex] = { "UV3", 0, DXGI_FORMAT_R32G32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
		}

        if (HasAttribute(EAttribute::Tangent))
        {
			m_InputElementDesc[AttributeIndex] = { "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
        }
            
        if (HasAttribute(EAttribute::Bitangent))
        {
			m_InputElementDesc[AttributeIndex] = { "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, AttributeIndex, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			AttributeIndex++;
        }

		return m_InputElementDesc;
	}

	Mesh::Mesh(std::string AssetPath, std::string AssetName)
		: GameAsset(AssetPath, AssetName)
	{
	}

	bool Mesh::LoadAsset()
	{
		return false;
	}

	bool Mesh::SaveAsset()
	{
		return false;
	}

	void Mesh::RenderOptions()
	{
		ImGui::Text("Mesh info:");
	}
}
