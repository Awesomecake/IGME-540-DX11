#include "Lighting.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    float4 surfaceColor;
}

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState BasicSampler : register(s0);

Texture2D ShadowMap : register(t4);
SamplerComparisonState ShadowSampler : register(s1);
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
    // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
    // Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    // Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    // Get a ratio of comparison results using SampleCmpLevelZero()
    float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, distToLight).r;

    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1);
    
    float3 albedoColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    float3 specularColor = lerp(F0_NON_METAL, albedoColor.rgb, metalness);
    
    input.normal = normalize(input.normal);
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - input.normal * dot(T, input.normal)); // Gram-Schmidt assumes T&N are normalized!
    input.normal = mul(unpackedNormal, float3x3(T, cross(T, input.normal), input.normal)); // Note multiplication order!
        
    float3 totalLight = CalcLights(input, surfaceColor.xyz,specularColor,roughness,metalness, shadowAmount);
 
    return float4(pow(surfaceColor.xyz * albedoColor * totalLight, 1.0f / 2.2f), 1);
}