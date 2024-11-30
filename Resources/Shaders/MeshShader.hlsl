
struct SceneData
{
    float4x4 World[128];
    float4x4 WorldView[128];
    float4x4 WorldViewProj[128];
    uint ShadingVariant;
    /*
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
    uint ShadingVariant;
    */
};

struct VertexOut
{
    float4 PositionHomogenousSpace : SV_POSITION;
    float3 Color : COLOR0;
    //float3 PositionViewSpace : POSITION0;
    //float3 Normal : NORMAL0;
    //float2 UVs : UV0;
};

struct Meshlet
{
    uint VertexCount;
    uint VertexOffset;
    uint PrimitiveCount;
    uint PrimitiveOffset;
};

struct Vertex
{
    float3 Position;
    float3 Normal;
    //float2 UVs;
};

cbuffer RootConstants : register(b0)
{
    uint PrimitiveID;
};
ConstantBuffer<SceneData> Globals : register(b1);

StructuredBuffer<Vertex> MeshVertices : register(t0);
StructuredBuffer<Meshlet> Meshlets : register(t1);
StructuredBuffer<uint> MeshletVertexIndices : register(t2);
StructuredBuffer<uint> MeshletTriangles : register(t3);

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint GroupThreadID : SV_GroupThreadID,
    uint GroupID : SV_GroupID,
    out indices uint3 Triangles[126],
    out vertices VertexOut Vertices[64]
)
{
    Meshlet ProcessedMeshlet = Meshlets[GroupID];
    
    SetMeshOutputCounts(ProcessedMeshlet.VertexCount, ProcessedMeshlet.PrimitiveCount);

    if (GroupThreadID < ProcessedMeshlet.PrimitiveCount)
    {
        uint Packed = MeshletTriangles[ProcessedMeshlet.PrimitiveOffset + GroupThreadID];
        Triangles[GroupThreadID] = uint3((Packed >> 0) & 0x3FF, (Packed >> 10) & 0x3FF, (Packed >> 20) & 0x3FF);
    }

    if (GroupThreadID < ProcessedMeshlet.VertexCount)
    {
        uint VertexIndex = ProcessedMeshlet.VertexOffset + GroupThreadID;        
        VertexIndex = MeshletVertexIndices[VertexIndex];
        
        Vertices[GroupThreadID].PositionHomogenousSpace = mul(float4(MeshVertices[VertexIndex].Position, 1.0), Globals.WorldViewProj[PrimitiveID]);
        
        float3 Color = float3(0.0, 1.0, 0.0);
        if (Globals.ShadingVariant == 0)
        {
            Color = float3(
                float(GroupID & 1),
                float(GroupID & 3) / 4,
                float(GroupID & 7) / 8);
        }
        else if (Globals.ShadingVariant == 1)
        {
            Color = float3(
                float(GroupThreadID & 1),
                float(GroupThreadID & 3) / 4,
                float(GroupThreadID & 7) / 8);
        }
         
        Vertices[GroupThreadID].Color = Color;
    }
}