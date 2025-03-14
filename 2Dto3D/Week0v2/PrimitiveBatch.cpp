#include "PrimitiveBatch.h"
#include "EngineLoop.h"
UPrimitiveBatch::UPrimitiveBatch()
{
}

UPrimitiveBatch::~UPrimitiveBatch()
{
    if (vertexBuffer) {
        vertexBuffer->Release();
        vertexBuffer = nullptr;
    }
}

void UPrimitiveBatch::AddLine(const FVector& start, const FVector& end, const FVector4& color)
{
	Vertices.push_back({ start.x, start.y, start.z, 
						 color.x, color.y, color.z, 1 });

	Vertices.push_back({ end.x, end.y, end.z,
						 color.x, color.y, color.z, 1 });
}

void UPrimitiveBatch::AddGridLine(const FVector& start, const FVector& end, const FVector4& color)
{

    GridVertices.push_back({ start.x, start.y, start.z,
                         color.x, color.y, color.z, 1 });

    GridVertices.push_back({ end.x, end.y, end.z,
                         color.x, color.y, color.z, 1 });
}

void UPrimitiveBatch::Begin()
{
	Vertices.clear();
}

void UPrimitiveBatch::AddGrid(int gridSize)
{
    if (Spacing != PreSpacing) {
        for (int i = -gridSize; i <= gridSize; i++) {
            float pos = i * Spacing;
        
            // y-up 기준
            // 세로선
            AddGridLine({ pos, -gridSize * Spacing, 0 }, { pos, gridSize * Spacing, 0 }, { 0.5f, 0.5f, 0.5f, 1.0f });

            // 가로선
            AddGridLine({ -gridSize * Spacing,  pos, 0 }, { gridSize * Spacing,  pos, 0 }, { 0.5f, 0.5f, 0.5f, 1.0f });
        }
        PreSpacing = Spacing;
    }
}
void UPrimitiveBatch::SetSpacing(float spacing)
{
    Spacing = spacing;
}

void UPrimitiveBatch::End(const FMatrix& View, const FMatrix& Projection)
{
    Vertices.insert(Vertices.end(), GridVertices.begin(), GridVertices.end());
    if (Vertices.empty()) return; // 렌더링할 데이터가 없으면 패스

    if (Vertices.size() > allocatedCapacity) {
        if (vertexBuffer) {
            vertexBuffer->Release();
        }
        allocatedCapacity = Vertices.size() * 2;
        vertexBuffer = FEngineLoop::renderer.CreateDynamicBatchBuffer(allocatedCapacity);
    }

    FEngineLoop::renderer.UpdateBuffer(vertexBuffer, Vertices);


    UINT stride = sizeof(FVertexSimple);
    UINT offset = 0;
    FMatrix Model = FMatrix::Identity;

    FMatrix MVP = Model * View * Projection;
    FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);
    FEngineLoop::renderer.RenderBatch(vertexBuffer, Vertices.size(), stride, offset);
}

void UPrimitiveBatch::AddBox(const FVector& center, const FVector4& color, const FMatrix& modelMatrix)
{  
    FVector half = {1,1,1};
    FVector localVertices[8] = {
        FVector(-half.x, -half.y, -half.z),
        FVector(half.x, -half.y, -half.z),
        FVector(half.x,  half.y, -half.z),
        FVector(-half.x,  half.y, -half.z),
        FVector(-half.x, -half.y,  half.z),
        FVector(half.x, -half.y,  half.z),
        FVector(half.x,  half.y,  half.z),
        FVector(-half.x,  half.y,  half.z)
    };

    // OBB의 월드 좌표 버텍스 계산
    FVector worldVertices[8];
    for (int i = 0; i < 8; i++) {
        worldVertices[i] = center + FMatrix::TransformVector(localVertices[i], modelMatrix);
    }

    // AABB의 최소, 최대값 계산 (삼항 연산자 사용)
    FVector min = worldVertices[0];
    FVector max = worldVertices[0];
    for (int i = 1; i < 8; i++) {
        min.x = (worldVertices[i].x < min.x) ? worldVertices[i].x : min.x;
        min.y = (worldVertices[i].y < min.y) ? worldVertices[i].y : min.y;
        min.z = (worldVertices[i].z < min.z) ? worldVertices[i].z : min.z;

        max.x = (worldVertices[i].x > max.x) ? worldVertices[i].x : max.x;
        max.y = (worldVertices[i].y > max.y) ? worldVertices[i].y : max.y;
        max.z = (worldVertices[i].z > max.z) ? worldVertices[i].z : max.z;
    }

    // AABB의 8개 버텍스 구성
    FVector aabbVertices[8] = {
        FVector(min.x, min.y, min.z),
        FVector(max.x, min.y, min.z),
        FVector(max.x, max.y, min.z),
        FVector(min.x, max.y, min.z),
        FVector(min.x, min.y, max.z),
        FVector(max.x, min.y, max.z),
        FVector(max.x, max.y, max.z),
        FVector(min.x, max.y, max.z)
    };

    // 밑면 에지: 0-1, 1-2, 2-3, 3-0
    AddLine(aabbVertices[0], aabbVertices[1], color);
    AddLine(aabbVertices[1], aabbVertices[2], color);
    AddLine(aabbVertices[2], aabbVertices[3], color);
    AddLine(aabbVertices[3], aabbVertices[0], color);

    // 윗면 에지: 4-5, 5-6, 6-7, 7-4
    AddLine(aabbVertices[4], aabbVertices[5], color);
    AddLine(aabbVertices[5], aabbVertices[6], color);
    AddLine(aabbVertices[6], aabbVertices[7], color);
    AddLine(aabbVertices[7], aabbVertices[4], color);

    // 측면 에지: 0-4, 1-5, 2-6, 3-7
    AddLine(aabbVertices[0], aabbVertices[4], color);
    AddLine(aabbVertices[1], aabbVertices[5], color);
    AddLine(aabbVertices[2], aabbVertices[6], color);
    AddLine(aabbVertices[3], aabbVertices[7], color);

}


void UPrimitiveBatch::AddCone(const FVector& center, float radius, float height, int segments, const FVector4& color, const FMatrix& modelMatrix)
{
    // 로컬 좌표에서 꼭짓점은 (0,0,0)으로 정의됨
    FVector localApex = FVector(0, 0, 0);
    // 월드 좌표상의 꼭짓점 (모델 행렬 변환 및 center 오프셋 적용)
    FVector worldApex = center + FMatrix::TransformVector(localApex, modelMatrix);

    float angleStep = 2 * PI / segments;
    TArray<FVector> basePoints;
    basePoints.reserve(segments);

    // 밑면 원의 각 점을 로컬 좌표에서 계산 (z = height에서 원을 형성)
    for (int i = 0; i < segments; i++)
    {
        float angle = i * angleStep;
        // 로컬 좌표상의 밑면 점: z 좌표가 height
        FVector localPoint = FVector(cos(angle) * radius, sin(angle) * radius, height);
        // 월드 좌표로 변환
        basePoints.push_back(center + FMatrix::TransformVector(localPoint, modelMatrix));
    }

    // 밑면 원 둘레를 이루는 선분 추가
    for (int i = 0; i < segments; i++)
    {
        int next = (i + 1) % segments;
        AddLine(basePoints[i], basePoints[next], color);
    }

    // 꼭짓점(아펙스)와 밑면 각 점을 연결하는 선분 추가 (측면)
    for (int i = 0; i < segments; i++)
    {
        AddLine(worldApex, basePoints[i], color);
    }
}
