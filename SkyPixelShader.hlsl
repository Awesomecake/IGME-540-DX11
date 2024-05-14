struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

cbuffer ConstantBuffer : register(b0)
{
    float3 ambient;
}

TextureCube cubeMap : register(t0);
SamplerState SkyBoxSampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    return float4(ambient, 1) * cubeMap.Sample(SkyBoxSampler, input.sampleDir);
}