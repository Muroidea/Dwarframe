
struct Constants
{
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
    uint DrawMeshlets;
};

struct VertexOut
{
    float4 PositionHomogenousSpace : SV_POSITION;
    float3 Color : COLOR0;
    //float3 PositionViewSpace : POSITION0;
    //float3 Normal : NORMAL0;
    //float2 UVs : UV0;
    uint MeshletIndex : COLOR1;
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

ConstantBuffer<Constants> Globals : register(b0);

StructuredBuffer<Vertex> MeshVertices : register(t0);
StructuredBuffer<Meshlet> Meshlets : register(t1);
StructuredBuffer<uint> MeshletVertexIndices : register(t2);
StructuredBuffer<uint> MeshletTriangles : register(t3);

/////
// Data Loaders
/*
uint3 UnpackPrimitive(uint Primitive)
{
    // Unpacks a 10 bits per index triangle from a 32-bit uint.
    return uint3(Primitive & 0x3FF, (Primitive >> 10) & 0x3FF, (Primitive >> 20) & 0x3FF);
}

uint3 GetPrimitive(Meshlet InMeshlet, uint Index)
{
    return UnpackPrimitive(MeshletTriangles[InMeshlet.PrimitiveOffset + Index]);
}

uint GetVertexIndex(Meshlet InMeshlet, uint LocalIndex)
{
    LocalIndex = InMeshlet.VertexOffset + LocalIndex;
    return MeshletVertexIndices.Load(LocalIndex);
}

VertexOut GetVertexAttributes(uint MeshletIndex, uint VertexIndex)
{
    Vertex v = MeshVertices[VertexIndex];

    VertexOut VOut;
    VOut.PositionViewSpace = mul(float4(v.Position, 1), Globals.WorldView).xyz;
    VOut.PositionHomogenousSpace = mul(float4(v.Position, 1), Globals.WorldViewProj);
    VOut.Normal = mul(float4(v.Normal, 0), Globals.World).xyz;
    VOut.UVs = v.UVs;
    VOut.MeshletIndex = MeshletIndex;

    return VOut;
}
*/
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
        uint vIdx0  = (Packed >>  0) & 0x3FF;
        uint vIdx1  = (Packed >> 10) & 0x3FF;
        uint vIdx2  = (Packed >> 20) & 0x3FF;
        Triangles[GroupThreadID] = uint3(vIdx0, vIdx1, vIdx2);

        //Triangles[GroupThreadID] = GetPrimitive(ProcessedMeshlet, GroupThreadID);
    }

    if (GroupThreadID < ProcessedMeshlet.VertexCount)
    {
        uint VertexIndex = ProcessedMeshlet.VertexOffset + GroupThreadID;        
        VertexIndex = MeshletVertexIndices[VertexIndex];

        Vertices[GroupThreadID].PositionHomogenousSpace = mul(float4(MeshVertices[VertexIndex].Position, 1.0), Globals.WorldViewProj);
        Vertices[GroupThreadID].MeshletIndex = GroupID;
        
        float3 Color = float3(
            float(GroupThreadID & 1),
            float(GroupThreadID & 3) / 4,
            float(GroupThreadID & 7) / 8);

        Vertices[GroupThreadID].Color = Color;

        //uint VertexIndex = GetVertexIndex(ProcessedMeshlet, GroupThreadID);
        //Vertices[GroupThreadID] = GetVertexAttributes(GroupID, VertexIndex);
    }
}