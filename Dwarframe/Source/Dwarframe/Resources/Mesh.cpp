#include "pch.h"
#include "Mesh.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

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

	Submesh::~Submesh()
	{
#if !MESH_USE_VECTOR

		if (MeshVertices)
		{
			free(MeshVertices);
			MeshVertices = nullptr;
		}

		if (bUseSmallIndices)
		{
			if (SmallIndices)
			{
				free(SmallIndices);
				SmallIndices = nullptr;
			}
		}
		else
		{
			if (BigIndices)
			{
				free(BigIndices);
				BigIndices = nullptr;
			}
		}

		if (Meshlets)
		{
			free(Meshlets);
			Meshlets = nullptr;
			
			free(MeshletTriangles);
			MeshletTriangles = nullptr;

			free(MeshletVertexIndices);
			MeshletVertexIndices = nullptr;
		}

#endif
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
#if WITH_EDITOR
	void Mesh::RenderProperties()
	{
		ImGui::Text("Mesh info:");
	}
#endif
}
