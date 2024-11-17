#ifndef MESH_H
#define MESH_H

#include "GameAsset.h"

namespace Dwarframe {

    constexpr uint32 MaxNumberOfTextureCoords = 4;

    enum EAttribute : uint16
    {
        Position = 0,
		Normal,
		Tangent,
		Bitangent,
        TexCoord_0,
        TexCoord_1,
        TexCoord_2,
        TexCoord_3,
        InstanceMatrix,
        Count
    };

    inline constexpr uint32_t GetAttributeBitValue(EAttribute Attribute)
    {
        return 1 << Attribute;
    }

	inline uint32 GetAttributeNumOfFloats(EAttribute Attribute)
	{
		switch (Attribute)
		{
		case EAttribute::Position:
		case EAttribute::Normal:
		case EAttribute::Tangent:
		case EAttribute::Bitangent:
			return 3;
		case EAttribute::TexCoord_0:
		case EAttribute::TexCoord_1:
		case EAttribute::TexCoord_2:
		case EAttribute::TexCoord_3:
			return 2;
		case EAttribute::InstanceMatrix:
			return 16;
		}
	}

    inline uint32 GetAttributeSizeInBytes(EAttribute Attribute)
    {
        switch (Attribute)
        {
            case EAttribute::Position:
            case EAttribute::Normal:
            case EAttribute::Tangent:
            case EAttribute::Bitangent:
                return 3 * sizeof(float32);
            case EAttribute::TexCoord_0:
            case EAttribute::TexCoord_1:
            case EAttribute::TexCoord_2:
            case EAttribute::TexCoord_3:
                return 2 * sizeof(float32);
            case EAttribute::InstanceMatrix:
                return 16 * sizeof(float32);
        }
    }

    struct VertexLayout
    {
    public:
        ~VertexLayout();
        
        inline void AddAttribute(EAttribute Attr) 
		{
			bool bNewAttribute = !(bool)(m_AttributeFlags & GetAttributeBitValue(Attr));
            m_NumOfAttributes += bNewAttribute;
            m_VertexStrideInBytes += bNewAttribute * GetAttributeSizeInBytes(Attr);
            //m_VertexStrideInFloats += bNewAttribute * GetAttributeNumOfFloats(Attr);
            m_AttributeFlags |= GetAttributeBitValue(Attr);
        }

        inline void RemoveAttribute(EAttribute Attr)
        {
            bool bOldAttribute = (bool)(m_AttributeFlags & GetAttributeBitValue(Attr));
			m_NumOfAttributes -= bOldAttribute;
			m_VertexStrideInBytes -= bOldAttribute * GetAttributeSizeInBytes(Attr);
			//m_VertexStrideInFloats -= bOldAttribute * GetAttributeNumOfFloats(Attr);
            m_AttributeFlags &= ~GetAttributeBitValue(Attr); 
        }

        inline bool HasAttribute(EAttribute Attr) const { return m_AttributeFlags & GetAttributeBitValue(Attr); }
        inline uint8 GetNumOfAttributes() const { return m_NumOfAttributes; }
        inline uint8 GetVertexStrideInBytes() const { return m_VertexStrideInBytes; }
        inline uint8 GetVertexStrideInFloats() const { return m_VertexStrideInBytes; }

        D3D12_INPUT_ELEMENT_DESC* GetInputElementDesc();

    private:
        D3D12_INPUT_ELEMENT_DESC* m_InputElementDesc = nullptr;
        uint16 m_AttributeFlags = GetAttributeBitValue(EAttribute::Position);
        uint8 m_NumOfAttributes = 1;
        uint8 m_VertexStrideInBytes = GetAttributeSizeInBytes(EAttribute::Position);
        //uint8 m_VertexStrideInFloats = GetAttributeNumOfFloats(EAttribute::Position);
    };
    /*
    class Model
    {
        friend class MeshLoader;

        struct MeshEntry
        {
            uint32_t NumIndices = 0;
            uint32_t BaseVertex = 0;
            uint32_t BaseIndex = 0;
            uint32_t MaterialIndex = 0;
        };

        struct MeshData
        {
            // TODO:  We should use float* and store all data in one big buffer
            std::vector<DirectX::XMFLOAT3> m_Positions;
            std::vector<DirectX::XMFLOAT3> m_Normals;
            std::vector<DirectX::XMFLOAT2> m_UVs;
            std::vector<DirectX::XMFLOAT3> m_Tangents;
            std::vector<DirectX::XMFLOAT3> m_Bitangents;
            std::vector<Buffer*> m_AttributeBuffers;
            
            std::vector<uint32_t> m_Indices;
            Buffer* m_IndicesBuffer;
        };

        struct MeshletData
        {
		    struct MeshletVertex
		    {
			    DirectX::XMFLOAT3 Position;
			    DirectX::XMFLOAT3 Normal;
			    DirectX::XMFLOAT2 UV0;
		    };

            std::vector<DirectX::Meshlet> m_Meshlets;
            std::vector<DirectX::MeshletTriangle> m_MeshletTriangles;
            std::vector<uint32_t> m_VertexIndices;
		    std::vector<MeshletVertex> m_Vertices;

            Buffer* m_VerticesBuffer;
            Buffer* m_MeshletTrianglesBuffer;
            Buffer* m_VertexIndicesBuffer;
            Buffer* m_MeshletsBuffer;
        };
        
    public:
        inline void SetName(std::wstring NewName) { m_Name = NewName; }
        inline void SetVertexLayout(VertexLayout* Layout) { m_VertexLayout = Layout; }

        inline uint32_t GetSize() const
        {
            return m_Entries.size();
        }

        inline void Resize(uint32_t NumOfMeshes)
        {
            m_Entries.resize(NumOfMeshes);
        }
        
        inline void Resize(uint32_t NumOfVertices, uint32_t NumOfIndices)
        {
            m_MeshData.m_Indices.reserve(NumOfIndices);

            if (m_VertexLayout->HasAttribute(EAttribute::Position))
            {
                m_MeshData.m_Positions.resize(NumOfVertices);
            }
            
            if (m_VertexLayout->HasAttribute(EAttribute::Normal))
            {
                m_MeshData.m_Normals.resize(NumOfVertices);
            }
            
            if (m_VertexLayout->HasAttribute(EAttribute::TexCoord_0))
            {
                m_MeshData.m_UVs.reserve(NumOfVertices);
            }
            
            if (m_VertexLayout->HasAttribute(EAttribute::Tangent))
            {
                m_MeshData.m_Tangents.resize(NumOfVertices);
            }
            
            if (m_VertexLayout->HasAttribute(EAttribute::Bitangent))
            {
                m_MeshData.m_Bitangents.resize(NumOfVertices);
            }
        }

        std::vector<MeshEntry> m_Entries;
        VertexLayout* m_VertexLayout;

        void FillTransitionBarrierToNewStateMeshlets(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState);
        void FillTransitionBarrierToNewStateTextures(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState);
        void SubmitMeshletDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer);
        void SubmitTextureDataToUpload(UploadBuffer* InUploadBuffer);
        void BindMeshletData(ID3D12GraphicsCommandList6* CommandList);

        size_t GetMeshletsDataSize() const;
        size_t GetTexturesDataSize() const;
        uint32_t GeNumOfMeshlets() { return m_MeshletData.m_Meshlets.size(); }

    private:
        MeshData m_MeshData;
        MeshletData m_MeshletData;

        std::wstring m_Name;
        
        TextureLoader::TextureData m_DiffuseTextureData;
        Texture2D* m_DiffuseTexture;

        TextureLoader::TextureData m_NormalTextureData;
        Texture2D* m_NormalTexture;
    };

	class Mesh
	{
        friend class MeshLoader;
        struct Submesh;

    public:
        void FillTransitionBarrierToNewState(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState);
        void SubmitDataToUpload(UploadBuffer* InUploadBuffer);
        void CreateGPUData(ID3D12Device8* Device);
        
        void FillTransitionBarrierToNewStateMeshletsData(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState);
        void SubmitMeshletDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer);
        void BindMeshletData(ID3D12GraphicsCommandList6* CommandList);

        void DrawMeshes(ID3D12GraphicsCommandList6* CommandList, uint32_t NumOfInstances, uint32_t InstanceBase = 0);
        
        inline void SetName(std::wstring NewName) { m_Name = NewName; }
        inline void SetVertexLayout(VertexLayout* Layout) { m_VertexLayout = Layout; }
        
        inline uint32_t GetNumOfMeshletsTriangles(uint32_t SubmeshID) const { return m_Submeshes[SubmeshID].m_MeshletTriangles.size(); }
        inline uint32_t GetNumOfMeshlets(uint32_t SubmeshID) const { return m_Submeshes[SubmeshID].m_Meshlets.size(); }

        uint32_t GetSizeInBytes() const;
        uint32_t GetSizeInBytes(uint32_t SubmeshID) const;
        uint32_t GetMeshletDataSizeInBytes(uint32_t SubmeshID) const;

        inline uint32_t GetNumOfNeededBarriers() const { return m_Submeshes.size() * m_Submeshes[0].m_AttributeBuffers.size() + 1; }
        inline bool HasAttribute(EAttribute Attribute) const { return m_VertexLayout->HasAttribute(Attribute); }
        
        inline size_t GetNumOfIndices() const { return m_Indices.size(); }
        inline size_t GetNumOfIndices(uint32_t SubmeshID) const { return m_Submeshes[SubmeshID].m_IndicesCount; }
        inline size_t GenNumOfVertices(uint32_t SubmeshID) const { return m_Submeshes[SubmeshID].m_Positions.size(); }

        inline uint32_t GetVerticesDataSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].Vertices.size() * sizeof(Submesh::MeshletVertex); }
        inline uint32_t GetMeshletTrianglesSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_MeshletTriangles.size() * sizeof(DirectX::MeshletTriangle); }
        inline uint32_t GetUniqueIndicesSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_VertexIndices.size() * sizeof(uint8_t); }
        inline uint32_t GetMeshletsSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_Meshlets.size() * sizeof(DirectX::Meshlet); }
        
        inline uint32_t GetPositionsSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_Positions.size() * sizeof(DirectX::XMFLOAT3); }
        inline uint32_t GetNormalsSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_Normals.size() * sizeof(DirectX::XMFLOAT3); }
        inline uint32_t GetUVsSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_UVs.size() * sizeof(DirectX::XMFLOAT2); }
        inline uint32_t GetTangentsSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_Tangents.size() * sizeof(DirectX::XMFLOAT3); }
        inline uint32_t GetBitangentsSizeInBytes(uint32_t SubmeshIndex) const { return m_Submeshes[SubmeshIndex].m_Bitangents.size() * sizeof(DirectX::XMFLOAT3); }

        inline uint32_t GetIndicesSizeInBytes() const { return m_Indices.size() * sizeof(uint32_t); }

    private:
        struct Submesh
        {
            // TODO: Right now all submeshes of single mesh have to have the same vertex layout - is it bad?
            
            //float* SubmeshData;
            //std::vector<uint32_t>

            // TODO:  We should use float* and store all data in one big buffer
            std::vector<DirectX::XMFLOAT3> m_Positions;
            std::vector<DirectX::XMFLOAT3> m_Normals;
            std::vector<DirectX::XMFLOAT2> m_UVs;
            std::vector<DirectX::XMFLOAT3> m_Tangents;
            std::vector<DirectX::XMFLOAT3> m_Bitangents;
            
            std::vector<Buffer*> m_AttributeBuffers;

            // TODO:  It should use uint8_t and then we can manage it properly
            uint32_t m_IndicesOffset;
            uint32_t m_IndicesCount;

            std::vector<DirectX::MeshletTriangle> m_MeshletTriangles;
            std::vector<uint32_t> m_VertexIndices;
            std::vector<DirectX::Meshlet> m_Meshlets;
            
		    struct MeshletVertex
		    {
			    DirectX::XMFLOAT3 Position;
			    DirectX::XMFLOAT3 Normal;
		    };
		    std::vector<MeshletVertex> Vertices;

            Buffer* m_VerticesBuffer;
            Buffer* m_MeshletTrianglesBuffer;
            Buffer* m_VertexIndicesBuffer;
            Buffer* m_MeshletsBuffer;
        };

        inline Submesh& AddSubmesh()
        {
            m_Submeshes.emplace_back();
            return m_Submeshes.back();
        }
        
        void ResizeSubmesh(Submesh& Mesh, uint32_t VerticesCount, uint32_t IndicesCount);

        inline void SetIndex(uint32_t Index, uint32_t Value) { m_Indices[Index] = Value; }

    private:
        VertexLayout* m_VertexLayout;

        std::vector<Submesh> m_Submeshes;

        std::vector<uint32_t> m_Indices;
        Buffer* m_IndicesBuffer;

        std::wstring m_Name;
	};

    */
    


    struct Submesh
	{
		struct Meshlet
		{
			uint32 VertexCount;
			uint32 VertexOffset;

			uint32 TriangleCount;
			uint32 TriangleOffset;
		};

		struct MeshletTriangle
		{
			uint32 Index0 : 10;
			uint32 Index1 : 10;
			uint32 Index2 : 10;
		};

        uint32 NumOfIndices = 0;
        uint32 NumOfVertices = 0;

        bool bUseSmallIndices = false;
		uint8 MaterialIndex = 0;

		VertexLayout Attributes;

        // Vertex0 : Position | Normal | ... , 
        // Vertex1 : Position | Normal | ... , 
        // VertexN : Position | Normal | ...
        //float32* MeshVertices2;
		std::vector<float32> MeshVertices;

		std::vector<uint32> BigIndicesData;
        /*
        union
		{
			uint32* BigIndicesData2;
			uint16* SmallIndicesData;
        };
        */

		std::vector<Meshlet> Meshlets;
		std::vector<MeshletTriangle> MeshletTriangles;
		std::vector<uint32> MeshletVertexIndices;
    };

    class Mesh : public GameAsset
    {
    IMPLEMENT_SIMPLE_RTTI(Mesh)

    public:
        Mesh(std::string AssetPath, std::string AssetName);

        virtual bool LoadAsset() override;
        virtual bool SaveAsset() override;
        
#ifdef WITH_EDITOR
		virtual void RenderOptions() override;
#endif

        uint8 GetNumOfSubmeshes() const { return m_SubmeshList.size(); }
        Submesh& GetSubmesh(uint8 SubmeshID) { return m_SubmeshList[SubmeshID]; }

    private:
        //std::vector<Material> m_Materials;
        std::vector<Submesh> m_SubmeshList;

        friend class MeshLoader;
        friend class MeshManager;
    };
}

#endif // !MESH_H
