
struct Constants
{
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
    uint DrawMeshlets;
};

struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 UVs;
};

struct VertexOut
{
    float4 PositionHomogenousSpace : SV_Position;
    float3 PositionViewSpace : POSITION0;
    float3 Normal : NORMAL0;
    float2 UVs : UV0;
    uint MeshletIndex : COLOR0;
};

struct Meshlet
{
    uint VertexCount;
    uint VertexOffset;
    uint PrimitiveCount;
    uint PrimitiveOffset;
};

ConstantBuffer<Constants> Globals : register(b0);
float2 Chuj : register(b1);
float2 Cipa : register(b2);
float4 Dupa : register(b3);


StructuredBuffer<Vertex> MeshVertices : register(t0);
StructuredBuffer<Meshlet> Meshlets : register(t1);
StructuredBuffer<uint> VertexIndices : register(t2);
StructuredBuffer<uint> PrimitiveIndices : register(t3);

Buffer<uint> SomeData : register(t4);
Texture2DArray<float4> Textures : register(t5);
Texture2D<float4> DiffuseTexture : register(t6);
RWStructuredBuffer<Meshlet> SomeData3 :  register(u0);

sampler Sampler : register(s0);
/////
// Data Loaders

uint3 UnpackPrimitive(uint Primitive)
{
    // Unpacks a 10 bits per index triangle from a 32-bit uint.
    return uint3(Primitive & 0x3FF, (Primitive >> 10) & 0x3FF, (Primitive >> 20) & 0x3FF);
}

uint3 GetPrimitive(Meshlet InMeshlet, uint Index)
{
    return UnpackPrimitive(PrimitiveIndices[InMeshlet.PrimitiveOffset + Index]);
}

uint GetVertexIndex(Meshlet InMeshlet, uint LocalIndex)
{
    LocalIndex = InMeshlet.VertexOffset + LocalIndex;
    return VertexIndices.Load(LocalIndex);
}

VertexOut GetVertexAttributes(uint MeshletIndex, uint VertexIndex)
{
    Vertex v = MeshVertices[VertexIndex];

    VertexOut VOut;
    VOut.PositionViewSpace = mul(float4(v.Position, 1), Globals.WorldView).xyz;
    VOut.PositionHomogenousSpace = mul(float4(v.Position, 1), Globals.WorldViewProj);
    VOut.Normal = mul(float4(v.Normal, 0), Globals.World).xyz;
    float4 Res = DiffuseTexture.Load(v.UVs.x);
    Res = Textures.Load(int4(Cipa.x,Cipa.y,Chuj.x,Dupa.y));    
    VOut.UVs = Res.xz;
    //VOut.UVs = v.UVs;
    VOut.MeshletIndex = MeshletIndex;

    return VOut;
}

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint GroupThreadID : SV_GroupThreadID,
    uint GroupID : SV_GroupID,
    out indices uint3 Triangles[128],
    out vertices VertexOut Vertices[64]
)
{
    Meshlet ProcessedMeshlet = Meshlets[GroupID + SomeData[GroupID]];
    SomeData3[GroupID].VertexCount = SomeData[GroupID];
    SetMeshOutputCounts(ProcessedMeshlet.VertexCount, ProcessedMeshlet.PrimitiveCount);
    
    if (GroupThreadID < ProcessedMeshlet.VertexCount)
    {
        uint VertexIndex = VertexIndices.Load(ProcessedMeshlet.VertexOffset + GroupThreadID);
        Vertices[GroupThreadID] = GetVertexAttributes(GroupID, VertexIndex);
    }

    if (GroupThreadID < ProcessedMeshlet.PrimitiveCount)
    {
        Triangles[GroupThreadID] = GetPrimitive(ProcessedMeshlet, GroupThreadID);
    }
}