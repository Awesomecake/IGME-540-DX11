cbuffer externalData : register(b0)
{
    float sharpenAmount;
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
    float amount = 0.3 * sharpenAmount;

    float neighbor = amount * -1.0;
    float center = amount * 4.0 + 1.0;
    
    float offset = 0.0025;
    
    float3 color = Pixels.Sample(ClampSampler, (input.uv + float2(0, offset))).rgb * neighbor
      + Pixels.Sample(ClampSampler, (input.uv + float2(-offset, 0))).rgb * neighbor
      + Pixels.Sample(ClampSampler, (input.uv + float2(0, 0))).rgb * center
      + Pixels.Sample(ClampSampler, (input.uv + float2(offset, 0))).rgb * neighbor
      + Pixels.Sample(ClampSampler, (input.uv + float2(0, -offset))).rgb * neighbor;

    return float4(color, 1);
}