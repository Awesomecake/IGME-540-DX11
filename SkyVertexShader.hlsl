struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VertexToPixel_Sky
{
	float4 position: SV_POSITION;
    float3 sampleDir : DIRECTION;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;

    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    matrix vp = mul(projection, viewNoTranslation);
    output.position = mul(vp, float4(input.localPosition, 1.0f));
    output.position.z = output.position.w;
    
    output.sampleDir = input.localPosition;
    
    return output;
}