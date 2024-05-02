cbuffer externalData : register(b0)
{
    float pixelLevel;
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
    float pixelSize = 0.005 * pixelLevel+0.0001;
    float2 uv = input.uv;
    uv = float2(uv.r - (uv.r % pixelSize), uv.g - (uv.g % pixelSize));

    return Pixels.Sample(ClampSampler, uv);
}