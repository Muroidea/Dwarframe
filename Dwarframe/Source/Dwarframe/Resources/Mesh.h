#ifndef MESH_H
#define MESH_H

#include "GameAsset.h"

namespace Dwarframe {

    class Material;

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
        VertexLayout() = default;
        ~VertexLayout();
        
        inline void AddAttribute(EAttribute Attr) 
		{
			bool bNewAttribute = !(bool)(m_AttributeFlags & GetAttributeBitValue(Attr));
            m_NumOfAttributes += bNewAttribute;
            m_VertexStrideInBytes += bNewAttribute * GetAttributeSizeInBytes(Attr);
            m_AttributeFlags |= GetAttributeBitValue(Attr);
        }

        inline void RemoveAttribute(EAttribute Attr)
        {
            bool bOldAttribute = (bool)(m_AttributeFlags & GetAttributeBitValue(Attr));
			m_NumOfAttributes -= bOldAttribute;
			m_VertexStrideInBytes -= bOldAttribute * GetAttributeSizeInBytes(Attr);
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
    };

    struct Submesh
	{
        struct LODInfo
        {
            uint32 NumOfIndices;
            uint32 NumOfVertices;

            uint32 IndicesOffset = 0;
            uint32 VerticesOffset = 0;

            uint32 NumOfMeshlets;
            uint32 NumOfMeshletTriangles;
            uint32 NumOfMeshletVertexIndices;
            
            uint32 MeshletsOffset = 0;
            uint32 MeshletTrianglesOffset = 0;
            uint32 MeshletVertexIndicesOffset = 0;
        };

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

		struct MeshletBounds
		{
			float Center[3];
	        float Radius;

            float ConeApex[3];
	        float ConeAxis[3];
	        float ConeCutoff;
		};

        Submesh() = default;
        ~Submesh();
        
        inline uint32 GetLODNumOfIndices(uint32 LOD = 0) { return LODInfos[LOD].NumOfIndices; }
        inline uint32 GetLODNumOfVertices(uint32 LOD = 0) { return LODInfos[LOD].NumOfVertices; }
        
        inline uint32 GetLODNumOfMeshlets(uint32 LOD = 0) { return LODInfos[LOD].NumOfMeshlets; }
        inline uint32 GetLODNumOfMeshletTriangles(uint32 LOD = 0) { return LODInfos[LOD].NumOfMeshletTriangles; }
        inline uint32 GetLODNumOfMeshletVertexIndices(uint32 LOD = 0) { return LODInfos[LOD].NumOfMeshletVertexIndices; }

        inline uint32* GetLODIndices(uint32 LOD = 0) { return &BigIndicesV[LODInfos[LOD].IndicesOffset]; }
        inline float* GetLODVertices(uint32 LOD = 0) { return &MeshVerticesV[LODInfos[LOD].VerticesOffset]; }
        
        inline Meshlet* GetLODMeshlets(uint32 LOD = 0) { return &MeshletsV[LODInfos[LOD].MeshletsOffset]; }
        inline MeshletBounds* GetLODMeshletBounds(uint32 LOD = 0) { return &MeshletBoundsV[LODInfos[LOD].MeshletsOffset]; }
        inline MeshletTriangle* GetLODMeshletTriangles(uint32 LOD = 0) { return &MeshletTrianglesV[LODInfos[LOD].MeshletTrianglesOffset]; }
        inline uint32* GetLODMeshletVertexIndices(uint32 LOD = 0) { return &MeshletVertexIndicesV[LODInfos[LOD].MeshletVertexIndicesOffset]; }

        bool bUseSmallIndices = false;
		uint8 MaterialIndex = 0;

		VertexLayout Attributes;
        std::vector<LODInfo> LODInfos;
        /*
        uint32 NumOfMeshlets = 0;
        uint32 NumOfMeshletTriangles = 0;
        uint32 NumOfMeshletVertexIndices = 0;
        */
        // Vertex0 : Position | Normal | ... , 
        // Vertex1 : Position | Normal | ... , 
        // VertexN : Position | Normal | ...
        float32* MeshVertices = nullptr;

        union
		{
			uint32* BigIndices = nullptr;
			uint16* SmallIndices;
        };

        Meshlet* Meshlets = nullptr;
        MeshletTriangle* MeshletTriangles = nullptr;
        uint32* MeshletVertexIndices = nullptr;
        
        std::vector<float32> MeshVerticesV;
        std::vector<uint32> BigIndicesV;

		std::vector<Meshlet> MeshletsV;
		std::vector<MeshletBounds> MeshletBoundsV;
		std::vector<MeshletTriangle> MeshletTrianglesV;
		std::vector<uint32> MeshletVertexIndicesV;
    };

    class Mesh : public GameAsset
    {
    IMPLEMENT_SIMPLE_RTTI(Mesh)

    public:
        Mesh(std::string AssetPath, std::string AssetName);

        virtual bool LoadAsset() override;
        virtual bool SaveAsset() override;
        
#if WITH_EDITOR
        struct LODSetup
        {
            float Percentage;
            float Error;
        };

		virtual void RenderProperties() override;
#endif

        uint8 GetNumOfSubmeshes() const { return m_SubmeshList.size(); }
        Submesh& GetSubmesh(uint8 SubmeshID) { return m_SubmeshList[SubmeshID]; }

    private:
        std::vector<Material*> m_Materials;
        std::vector<Submesh> m_SubmeshList;

        friend class MeshLoader;
        friend class MeshManager;
    };
}

#endif // !MESH_H
