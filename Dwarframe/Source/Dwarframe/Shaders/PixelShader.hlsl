//#include "RootSignature.hlsl"
#include "Common.hlsli"

Correction CorrectionData : register(b0);
Texture2D<float4> textures[] : register(t0);
sampler sampler0 : register(s0);

struct VSOut
{
    float4 position : SV_Position;
    float3 normal : Normal;
    float2 uv0 : UV;
};

//[RootSignature(ROOTSIG)]
float4 main(VSOut Input) : SV_Target
{
    float4 texel = textures[0].Sample(sampler0, Input.uv0);
    //return float4(Input.color * color, 1.0f);
    //return float4(texel.rgb, 1.0f);
    return float4(Input.normal, 1.0f);
}