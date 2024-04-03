#include "GGP_Shader.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    float4 surfaceColor;
    float totalTime;
    float2 uvOffset;
    
    float3 ambient;
    Light lights[5];
    float roughness;
    float3 cameraPos;
}

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
Texture2D NormalMap : register(t1);
SamplerState BasicSampler : register(s0); // "s" registers for samplers

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
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv + uvOffset).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!
    
    // Feel free to adjust/simplify this code to fit with your existing shader(s)
    // Simplifications include not re-normalizing the same vector more than once!
    float3 N = normalize(input.normal); // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    // Assumes that input.normal is the normal later in the shader
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!
        
    float3 light = ambient;
    
    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                light += DirectionalLight(lights[i],input, surfaceColor.xyz,cameraPos,roughness);
                break;
            case LIGHT_TYPE_POINT:
                light += PointLight(lights[i],input,surfaceColor.xyz,cameraPos,roughness);
                break;
            case LIGHT_TYPE_SPOT:
                break;
        }
    }
    
    float3 textureColor = SurfaceTexture.Sample(BasicSampler, input.uv+uvOffset).rgb;
        
    return surfaceColor * float4(textureColor * light, 1);
}