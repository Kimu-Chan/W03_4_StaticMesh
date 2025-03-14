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
    float pad3[3]; // 16바이트 정렬 맞춤 추가 패딩
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
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // MVP 행렬을 사용한 위치 변환
    output.position = mul(input.position, MVP);
    
    // 색상 전달
    output.color = input.color;
    
    // 노멀은 정규화하여 전달
    output.normal = normalize(input.normal);
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // 표면 노멀 정규화
    float3 N = normalize(input.normal);
    
    // 조명 방향 계산 (directional light의 경우 빛이 들어오는 방향의 반대 사용)
    float3 L = normalize(-LightDirection);
    
    // Lambert 법칙에 따른 diffuse 계수 (0~1 사이 클램프)
    float diffuse = saturate(dot(N, L));
    
    // ambient와 diffuse 조명의 조합:
    // - ambient: AmbientFactor * 기본 색상
    // - diffuse: diffuse * LightColor * 기본 색상
    float3 litColor = AmbientFactor * input.color.rgb 
                    + diffuse * LightColor * input.color.rgb;
    
    return float4(litColor, input.color.a);
}
