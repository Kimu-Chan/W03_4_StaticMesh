Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET {

    return gTexture.Sample(gSampler, input.texCoord);
}
