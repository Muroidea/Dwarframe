//#include "RootSignature.hlsl"
struct Correction
{
    float3 Color;
    float pad4;
};

ConstantBuffer<Correction> CorrectionData : register(b0);

cbuffer Matrices : register(b1)
{
    float4x4 MVP[3];
};

struct VSIn
{
    float3 position : Position;
    float3 normal : Normal;
    float2 uv0 : UV;
    uint InstanceID : SV_InstanceID;
};

struct VSOut
{
    float4 position : SV_Position;
    float3 normal : Normal;
    float2 uv0 : UV;
};

//[RootSignature(ROOTSIG)]
VSOut main(VSIn Input)
{
    VSOut Output;
    Output.position = mul(MVP[Input.InstanceID], float4(Input.position, 1.0f));
    Output.normal = Input.normal;
    Output.uv0 = Input.uv0;

    return Output;
}