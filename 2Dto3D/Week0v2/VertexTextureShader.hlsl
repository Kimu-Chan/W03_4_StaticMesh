struct VSInput {
    float4 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer constants : register(b0)
{
    row_major float4x4 MVP;
    float Flag;
}

PSInput main(VSInput input) {


    PSInput output;
    output.position = output.position = mul(float4(input.position.xyz, 1.0f), MVP);
    
    output.texCoord = input.texCoord;
    
    return output;
}
