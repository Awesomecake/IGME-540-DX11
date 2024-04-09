#include "GGP_Shader.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    float4 surfaceColor;
    float totalTime;
    float2 mousePos;
}

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
    float uvShift = (input.uv.x + input.uv.y * (cos(totalTime/5)+1.2)*10) % 1;
    
    float startingDarkness = 0.1;
    
    float xDiff = input.screenPosition.x - mousePos.x;
    float yDiff = input.screenPosition.y - mousePos.y;
    
    float range = 100;
    float dist = length(float2(xDiff, yDiff));
    
    if (dist < range)
    {
        float brightness = (1 - dist / (range)) * (1 - startingDarkness) + startingDarkness;
        return surfaceColor * float4(brightness, brightness, brightness, 1) * float4(uvShift.xxx, 1);
    }

    //return surfaceColor * float4(xDiff.xxx,1);
    return surfaceColor * float4(startingDarkness, startingDarkness, startingDarkness, 1) * float4(uvShift.xxx, 1);
}