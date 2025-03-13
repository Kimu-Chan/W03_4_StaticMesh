struct VSInput {
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PSInput main(VSInput input) {
    PSInput output;
    output.position = float4(input.position, 1.0);
    output.texCoord = input.texCoord;
    return output;
}
