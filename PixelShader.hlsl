#include "Lighting.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    float4 surfaceColor;
    float totalTime;
}

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState BasicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{    
    input.normal = normalize(input.normal);
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!
    
    float3 albedoColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    float3 specularColor = lerp(F0_NON_METAL, albedoColor.rgb, metalness);
    
    // Feel free to adjust/simplify this code to fit with your existing shader(s)
    // Simplifications include not re-normalizing the same vector more than once!
    float3 N = normalize(input.normal); // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    // Assumes that input.normal is the normal later in the shader
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!
        
    float3 totalLight = CalcLights(input, surfaceColor.xyz,specularColor,roughness,metalness);
        
    return float4(pow(surfaceColor.xyz * albedoColor * totalLight, 1.0f / 2.2f), 1);
}