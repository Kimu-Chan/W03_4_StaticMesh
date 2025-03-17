
cbuffer MatrixBuffer : register(b0)
{
    row_major float4x4 MVP;
};

cbuffer GridParametersData : register(b1)
{
    float GridSpacing;
    int GridCount; // 총 grid 라인 수
    float3 GridOrigin; // Grid의 중심
    float Padding;
};
cbuffer PrimitiveCounts : register(b3)
{
    int BoundingBoxCount; // 렌더링할 AABB의 개수
    int pad;
    int ConeCount; // 렌더링할 cone의 개수
    int pad1;
};
struct FBoundingBoxData
{
    float3 bbMin;
    float padding0;
    float3 bbMax;
    float padding1;
};
struct FConeData
{
    float3 ConeApex; // 원뿔의 꼭짓점
    float padding0;
    
    float3 ConeBaseCenter; // 원뿔 밑면 중심
    float padding1;
   
    float ConeRadius; // 원뿔 밑면 반지름
    float ConeHeight; // 원뿔 높이 (Apex와 BaseCenter 간 차이)
    int ConeSegmentCount; // 원뿔 밑면 분할 수
    float padding2;
};

StructuredBuffer<FBoundingBoxData> g_BoundingBoxes : register(t2);
StructuredBuffer<FConeData> g_ConeData : register(t3);
static const int BB_EdgeIndices[12][2] =
{
    { 0, 1 },
    { 1, 3 },
    { 3, 2 },
    { 2, 0 }, // 앞면
    { 4, 5 },
    { 5, 7 },
    { 7, 6 },
    { 6, 4 }, // 뒷면
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 } // 측면
};

struct VS_INPUT
{
    uint vertexID : SV_VertexID; // 0 또는 1: 각 라인의 시작과 끝
    uint instanceID : SV_InstanceID; // 인스턴스 ID로 grid, axis, bounding box를 구분
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

/////////////////////////////////////////////////////////////////////////
// Grid 위치 계산 함수
/////////////////////////////////////////////////////////////////////////
float3 ComputeGridPosition(uint instanceID, uint vertexID)
{
    int halfCount = GridCount / 2;
    float centerOffset = halfCount * 0.5; // grid 중심이 원점에 오도록

    float3 startPos;
    float3 endPos;
    
    if (instanceID < halfCount)
    {
        // 수직선: X 좌표 변화, Y는 -centerOffset ~ +centerOffset
        float x = GridOrigin.x + (instanceID - centerOffset) * GridSpacing;
        startPos = float3(x, GridOrigin.y - centerOffset * GridSpacing, GridOrigin.z);
        endPos = float3(x, GridOrigin.y + centerOffset * GridSpacing, GridOrigin.z);
    }
    else
    {
        // 수평선: Y 좌표 변화, X는 -centerOffset ~ +centerOffset
        int idx = instanceID - halfCount;
        float y = GridOrigin.y + (idx - centerOffset) * GridSpacing;
        startPos = float3(GridOrigin.x - centerOffset * GridSpacing, y, GridOrigin.z);
        endPos = float3(GridOrigin.x + centerOffset * GridSpacing, y, GridOrigin.z);
    }
    return (vertexID == 0) ? startPos : endPos;
}

/////////////////////////////////////////////////////////////////////////
// Axis 위치 계산 함수 (총 3개: X, Y, Z)
/////////////////////////////////////////////////////////////////////////
float3 ComputeAxisPosition(uint axisInstanceID, uint vertexID)
{
    float3 start = float3(0.0, 0.0, 0.0);
    float3 end;
    float zOffset = 0.6f;
    if (axisInstanceID == 0)
    {
        // X 축: 빨간색
        end = float3(1000.0, 0.0, zOffset);
    }
    else if (axisInstanceID == 1)
    {
        // Y 축: 초록색
        end = float3(0.0, 1000.0, zOffset);
    }
    else if (axisInstanceID == 2)
    {
        // Z 축: 파란색
        end = float3(0.0, 0.0, 1000.0 + zOffset);
    }
    else
    {
        end = start;
    }
    return (vertexID == 0) ? start : end;
}

/////////////////////////////////////////////////////////////////////////
// Bounding Box 위치 계산 함수
// bbInstanceID: StructuredBuffer에서 몇 번째 bounding box인지
// edgeIndex: 해당 bounding box의 12개 엣지 중 어느 것인지
/////////////////////////////////////////////////////////////////////////
float3 ComputeBoundingBoxPosition(uint bbInstanceID, uint edgeIndex, uint vertexID)
{
    FBoundingBoxData box = g_BoundingBoxes[bbInstanceID];
  
//    0: (bbMin.x, bbMin.y, bbMin.z)
//    1: (bbMax.x, bbMin.y, bbMin.z)
//    2: (bbMin.x, bbMax.y, bbMin.z)
//    3: (bbMax.x, bbMax.y, bbMin.z)
//    4: (bbMin.x, bbMin.y, bbMax.z)
//    5: (bbMax.x, bbMin.y, bbMax.z)
//    6: (bbMin.x, bbMax.y, bbMax.z)
//    7: (bbMax.x, bbMax.y, bbMax.z)
    int vertIndex = BB_EdgeIndices[edgeIndex][vertexID];
    float x = ((vertIndex & 1) == 0) ? box.bbMin.x : box.bbMax.x;
    float y = ((vertIndex & 2) == 0) ? box.bbMin.y : box.bbMax.y;
    float z = ((vertIndex & 4) == 0) ? box.bbMin.z : box.bbMax.z;
    return float3(x, y, z);
}

/////////////////////////////////////////////////////////////////////////
// Axis 위치 계산 함수 (총 3개: X, Y, Z)
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Cone 계산 함수
/////////////////////////////////////////////////
// Helper: 계산된 각도에 따른 밑면 꼭짓점 위치 계산

float3 ComputeConePosition(uint globalInstanceID, uint vertexID)
{
    // 모든 cone이 동일한 세그먼트 수를 가짐
    int N = g_ConeData[0].ConeSegmentCount;
    
    uint coneIndex = globalInstanceID / (2 * N);
    uint lineIndex = globalInstanceID % (2 * N);
    
    // cone 데이터 읽기
    FConeData cone = g_ConeData[coneIndex];
    
    // cone의 축 계산
    float3 axis = normalize(cone.ConeApex - cone.ConeBaseCenter);
    
    // axis에 수직인 두 벡터(u, v)를 생성
    float3 arbitrary = abs(dot(axis, float3(0, 0, 1))) < 0.99 ? float3(0, 0, 1) : float3(0, 1, 0);
    float3 u = normalize(cross(axis, arbitrary));
    float3 v = cross(axis, u);
    
    if (lineIndex < (uint) N)
    {
        // 측면 선분: cone의 꼭짓점과 밑면의 한 점을 잇는다.
        float angle = lineIndex * 6.28318530718 / N;
        float3 baseVertex = cone.ConeBaseCenter + (cos(angle) * u + sin(angle) * v) * cone.ConeRadius;
        return (vertexID == 0) ? cone.ConeApex : baseVertex;
    }
    else
    {
        // 밑면 둘레 선분: 밑면상의 인접한 두 점을 잇는다.
        uint idx = lineIndex - N;
        float angle0 = idx * 6.28318530718 / N;
        float angle1 = ((idx + 1) % N) * 6.28318530718 / N;
        float3 v0 = cone.ConeBaseCenter + (cos(angle0) * u + sin(angle0) * v) * cone.ConeRadius;
        float3 v1 = cone.ConeBaseCenter + (cos(angle1) * u + sin(angle1) * v) * cone.ConeRadius;
        return (vertexID == 0) ? v0 : v1;
    }
}


/////////////////////////////////////////////////////////////////////////
// 메인 버텍스 셰이더
/////////////////////////////////////////////////////////////////////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float3 pos;
    float4 color;
    
    // 분기 기준:
    // Grid: instanceID 0 ~ (GridCount - 1)
    // Axis: instanceID GridCount ~ (GridCount + 2)
    // Bounding Box: 그 이후, 각 bounding box 당 12 인스턴스
    
    uint gridLineCount = GridCount;
    uint axisCount = 3;
    uint aabbInstanceCount = 12 * BoundingBoxCount;
    uint coneInstanceStart = gridLineCount + axisCount + aabbInstanceCount;
    
    if (input.instanceID < (uint) gridLineCount)
    {
        pos = ComputeGridPosition(input.instanceID, input.vertexID);
        color = float4(0.5, 0.5, 0.5, 1.0); // Grid: 회색
    }
    else if (input.instanceID < (uint) (gridLineCount + axisCount))
    {
        uint axisInstanceID = input.instanceID - gridLineCount;
        pos = ComputeAxisPosition(axisInstanceID, input.vertexID);
        if (axisInstanceID == 0)
            color = float4(1.0, 0.0, 0.0, 1.0); // X축: 빨강
        else if (axisInstanceID == 1)
            color = float4(0.0, 1.0, 0.0, 1.0); // Y축: 초록
        else if (axisInstanceID == 2)
            color = float4(0.0, 0.0, 1.0, 1.0); // Z축: 파랑
        else
            color = float4(1.0, 1.0, 1.0, 1.0);
    }
    else if (input.instanceID < (uint) (gridLineCount + axisCount + aabbInstanceCount))
    {
        uint index = input.instanceID - (gridLineCount + axisCount);
        uint bbInstanceID = index / 12;
        uint bbEdgeIndex = index % 12;
        pos = ComputeBoundingBoxPosition(bbInstanceID, bbEdgeIndex, input.vertexID);
        color = float4(1.0, 1.0, 0.0, 1.0); // Bounding Box: 노란색
    }
    else
    {
        uint coneInstanceID = input.instanceID - coneInstanceStart;
        pos = ComputeConePosition(coneInstanceID, input.vertexID);
        color = float4(1.0, 1.0, 1.0, 1.0);
    }
    
    output.Position = mul(float4(pos, 1.0), MVP);
    output.Color = color;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_Target
{
    return input.Color;
}
