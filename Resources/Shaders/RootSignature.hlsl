/*
*   Root signature layout (CPU)
*   - 0:    float3 "Color"
*   - 1:    Correction "Correction Data"
*   - 1:    descriptor table "Textures
*
*   Root signature layour (GPU)
*   - b0:       float3 "Color"
*   - b1:       ConstantBuffer        
*   - t0...     Texture2D<float4> "Textures"
*   - s0        Sampler for "Textures"
*   - s1        Sampler for "Textures"
*/

#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
"RootConstants(num32BitConstants=4, b0)," \
"DescriptorTable(" \
    "CBV(b1),"  \
    "SRV(t0, numDescriptors = unbounded)" \
")," \
"StaticSampler(s0)," \
"StaticSampler(s1, " \
              "addressU = TEXTURE_ADDRESS_CLAMP, "\
              "filter = FILTER_MIN_MAG_MIP_LINEAR)"