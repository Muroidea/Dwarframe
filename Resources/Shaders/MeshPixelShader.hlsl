
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

ConstantBuffer<Constants> Globals : register(b0);
//Texture2D<float4> DiffuseTexture : register(t6);
//Texture2D<float4> NormalTexture : register(t7);
//sampler Sampler : register(s0);

float4 main(VertexOut Input) : SV_TARGET
{
    /*
    float AmbientIntensity = 0.1;
    float3 LightColor = float3(1, 1, 1);
    float3 LightDir = -normalize(float3(1, -1, 1));

    float3 DiffuseColor;
    float Shininess;
    if (Globals.DrawMeshlets)
    {
        uint meshletIndex = Input.MeshletIndex;
        DiffuseColor = float3(
            float(meshletIndex & 1),
            float(meshletIndex & 3) / 4,
            float(meshletIndex & 7) / 8);
        Shininess = 16.0;
    }
    else
    {
        //DiffuseColor = DiffuseTexture.Sample(Sampler, Input.UVs).xyz;
        DiffuseColor = 0.8;
        Shininess = 64.0;
    }
    
    //float4 TNormal = NormalTexture.Sample(Sampler, Input.UVs);
    //float3 Normal = normalize(mul(float4(TNormal.xyz, 0), Globals.World).xyz);
    float3 Normal = float3(1.0, 0.0, 0.0);//normalize(Input.Normal);

    // Do some fancy Blinn-Phong shading!
    float CosAngle = saturate(dot(Normal, LightDir));
    float3 ViewDir = -normalize(Input.PositionViewSpace);
    float3 HalfAngle = normalize(LightDir + ViewDir);

    float BlinnTerm = saturate(dot(Normal, HalfAngle));
    
    if (CosAngle == 0.0f)
    {
        BlinnTerm = 0.0f;
    }
    
    //BlinnTerm = CosAngle != 0.0f ? BlinnTerm : 0.0f;
    BlinnTerm = pow(BlinnTerm, Shininess);

    float3 FinalColor = (CosAngle + BlinnTerm + AmbientIntensity) * DiffuseColor;

    return float4(FinalColor, 1);
    */
    float3 Color = float3(float(Input.MeshletIndex & 1),
            float(Input.MeshletIndex & 3) / 4,
            float(Input.MeshletIndex & 7) / 8);

    //return float4(Input.Color, 1.0);
    return float4(Color, 1.0);
}