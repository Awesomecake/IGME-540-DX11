#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__

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

float3 Specular(VertexToPixel input, float3 lightDirection, float3 cameraPos, float roughness)
{    
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    float3 V = normalize(cameraPos - input.worldPosition);
    float3 R = reflect(lightDirection, input.normal);
    
    float spec = 0;
    if (specExponent > 0.05)
        spec = pow(saturate(dot(R, V)), specExponent);
    
    return spec;
}

float3 Diffuse(float3 normal, float3 lightDirection)
{
    return saturate(dot(normal, -lightDirection));
}

float3 DirectionalLight(Light light, VertexToPixel input, float3 surfaceColor, float3 cameraPos, float roughness)
{
    light.Direction = normalize(light.Direction);

    float3 diffuse = Diffuse(input.normal, light.Direction);
    float3 specular = Specular(input, light.Direction, cameraPos, roughness);
    specular *= any(diffuse);
    return (diffuse*surfaceColor + specular) * light.Intensity * light.Color;
}


float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 PointLight(Light light, VertexToPixel input, float3 surfaceColor, float3 cameraPos, float roughness)
{   
    float3 normDir = normalize(light.Position - input.worldPosition);
    
    float3 attenuate = Attenuate(light, input.worldPosition);
    float3 diffuse = Diffuse(input.normal, normDir);
    float3 specular = Specular(input, normDir, cameraPos, roughness);
    specular *= any(diffuse);

    return (diffuse * surfaceColor + specular) * attenuate * light.Color * light.Intensity;
}

#endif