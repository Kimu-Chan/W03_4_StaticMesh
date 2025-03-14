// MatrixBuffer: 변환 행렬 관리
cbuffer MatrixBuffer : register(b0)
{
    row_major float4x4 MVP;
};

// LightingBuffer: 조명 관련 파라미터 관리
cbuffer LightingBuffer : register(b1)
{
    float3 LightDirection; // 조명 방향 (단위 벡터; 빛이 들어오는 방향의 반대 사용)
    float pad1; // 16바이트 정렬용 패딩
    float3 LightColor; // 조명 색상 (예: (1, 1, 1))
    float pad2; // 16바이트 정렬용 패딩
    float AmbientFactor; // ambient 계수 (예: 0.1)
    float pad3; // 16바이트 정렬 맞춤 추가 패딩
    float pad4; // 16바이트 정렬 맞춤 추가 패딩
    float pad5; // 16바이트 정렬 맞춤 추가 패딩
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float4 color : COLOR; // 전달할 색상
    float3 normal : NORMAL; // 정규화된 노멀 벡터
    float normalFlag : TEXCOORD0; // 노멀 유효성 플래그 (1.0: 유효, 0.0: 무효)
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 위치 변환
    output.position = mul(input.position, MVP);
    output.color = input.color;
    
    // 입력 normal 값의 길이 확인
    float normalThreshold = 0.001;
    float normalLen = length(input.normal);
    
    if (normalLen < normalThreshold)
    {
        output.normalFlag = 0.0;
    }
    else
    {
        //output.normal = normalize(input.normal);
        output.normal = mul(input.normal, MVP);
        output.normalFlag = 1.0;
    }
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // normal 값이 유효하지 않으면 흰색을 출력
    if (input.normalFlag < 0.5)
    {
        return float4(input.color.rgb, input.color.a);
    }
    
    // 정상적인 normal 값이 있을 때 조명 계산 진행
    float3 N = normalize(input.normal);
    float3 L = normalize(LightDirection);
    float diffuse = saturate(dot(N, L));

    // ambient와 diffuse 조명의 결합
    float3 litColor = AmbientFactor * input.color.rgb 
                    + diffuse * LightColor * input.color.rgb;
    
    return float4(litColor, input.color.a);
}
