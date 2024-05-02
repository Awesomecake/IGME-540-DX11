#ifndef __Lighting__ // Each .hlsli file needs a unique identifier!
#include "PBR.hlsli"
#define __Lighting__

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#define MAX_SPECULAR_EXPONENT 256.0f
#define MAX_NUM_LIGHTS 10

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;
    float4 shadowMapPos : SHADOW_POSITION;
};

struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotFalloff;
    float3 Padding;
};

cbuffer LightBuffer : register(b1)
{
    float3 ambient;
    Light lights[MAX_NUM_LIGHTS];
    float roughness;
    float3 cameraPos;
}

float3 DirectionalLight(Light light, VertexToPixel input, float3 surfaceColor, float3 toCam, float3 specColor, float roughness, float metalness)
{
    float diff = DiffusePBR(input.normal, -light.Direction);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, -light.Direction, toCam, roughness, specColor, F);
    // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    // Combine the final diffuse and specular values for this light
    return (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
}


float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 PointLight(Light light, VertexToPixel input, float3 surfaceColor, float3 toCam, float3 specColor, float roughness, float metalness)
{   
    float3 toLight = normalize(light.Position - input.worldPosition);

    // Calculate the light amounts
    float diff = DiffusePBR(input.normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, toLight, toCam, roughness, specColor, F);
    // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    // Combine the final diffuse and specular values for this light
    return (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
}

float3 CalcLights(VertexToPixel input, float3 surfaceColor, float3 specularColor, float roughness, float metalness, float shadowMapShadowAmount)
{
    float3 light;
    float3 toCam = normalize(cameraPos - input.worldPosition);

    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                float lightAdditive = DirectionalLight(lights[i], input, surfaceColor, toCam, specularColor, roughness, metalness);
                if (i == 0)
                {
                    lightAdditive *= shadowMapShadowAmount;
                }
                light += lightAdditive;
                break;
            case LIGHT_TYPE_POINT:
                light += PointLight(lights[i], input, surfaceColor, toCam, specularColor, roughness, metalness);
                break;
            case LIGHT_TYPE_SPOT:
                break;
        }
    }
    
    return light;
}

#endif