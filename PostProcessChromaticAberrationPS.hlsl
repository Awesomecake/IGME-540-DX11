cbuffer externalData : register(b0)
{
    float mouseX;
    float mouseY;
}

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float redOffset = 0.009;
    float greenOffset = 0.006;
    float blueOffset = -0.006;

    float2 direction = input.uv - float2(mouseX, mouseY);

    float4 output = Pixels.Sample(ClampSampler, input.uv);
    
    output.r = Pixels.Sample(ClampSampler, input.uv + (direction * float2(redOffset.rr))).r;
    output.g = Pixels.Sample(ClampSampler, input.uv + (direction * float2(greenOffset.rr))).g;
    output.b = Pixels.Sample(ClampSampler, input.uv + (direction * float2(blueOffset.rr))).b;
    
    return output;
}