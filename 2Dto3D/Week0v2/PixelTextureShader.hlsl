Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET {
    //return float4(input.texCoord.x, input.texCoord.y, 0, 1); // UV 좌표를 색상 값으로 출력

    return gTexture.Sample(gSampler, input.texCoord);
}
