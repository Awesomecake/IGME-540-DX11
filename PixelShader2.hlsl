
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
	float4 screenPosition	: SV_POSITION;
    float2 uv				: TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
    float4 colorTint;
    float totalTime;
    float2 mousePos;
}

float4 flashlight(float4 screenPosition)
{
        
    float startingDarkness = 0.1;
    
    float xDiff = screenPosition.x - mousePos.x;
    float yDiff = screenPosition.y - mousePos.y;
    
    float range = 100;
    float dist = length(float2(xDiff, yDiff));
    
    if (dist < range)
    {
        float brightness = (1 - dist / (range)) * (1 - startingDarkness) + startingDarkness;
        return float4(brightness, brightness, brightness, 1);
    }

    return float4(startingDarkness, startingDarkness, startingDarkness, 1);
}

float4 flashlight2(float4 screenPosition)
{

    float startingDarkness = 0.1;

    float xDiff = (screenPosition.x - mousePos.x);
    float yDiff = (screenPosition.y - mousePos.y);

    float range = 100;
    float dist = length(float2(xDiff, yDiff));

    if (dist < range)
    {
        float brightness = (1 - dist / (range)) * (1 - startingDarkness) + startingDarkness;
        return float4(brightness, brightness, brightness, 1);
    }

    return float4(startingDarkness, startingDarkness, startingDarkness, 1);
}

float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

float noise(float3 x)
{
        // The noise function returns a value in the range -1.0f -> 1.0f
    float3 p = floor(x);
    float3 f = frac(x);
     
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0 + 113.0 * p.z;
     
    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
               lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
               lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
               lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
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
    float xDiff = (input.uv.x + input.uv.y * (cos(totalTime/5)+1.2)*10) % 1;

    //return colorTint * float4(xDiff.xxx,1);
    return colorTint * flashlight2(input.screenPosition) * float4(xDiff.xxx, 1);
}