#include "PrimitiveBatch.h"
#include "EngineLoop.h"
#include "EditorViewportClient.h"
extern FEngineLoop GEngineLoop;

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
						 color.x, color.y, color.z, 1,0,0,0 });

	Vertices.push_back({ end.x, end.y, end.z,
						 color.x, color.y, color.z, 1,0,0,0 });
}

void UPrimitiveBatch::AddGridLine(const FVector& start, const FVector& end, const FVector4& color)
{

    GridVertices.push_back({ start.x, start.y, start.z,
                         color.x, color.y, color.z, 1,0,0,0 });

    GridVertices.push_back({ end.x, end.y, end.z,
                         color.x, color.y, color.z, 1,0,0,0 });
}

void UPrimitiveBatch::Begin()
{
	Vertices.clear();
}

void UPrimitiveBatch::AddGrid(int gridSize)
{
    float Spacing = GEngineLoop.GetViewportClient()->GetGridSize();
    if (Spacing != PreSpacing) {
        GridVertices.clear();
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
void UPrimitiveBatch::ClearGrid()
{
    PreSpacing = 0;
    GridVertices.clear();
}
//void UPrimitiveBatch::SetSpacing(float spacing)
//{
//    Spacing = spacing;
//}

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

void UPrimitiveBatch::AddBoxForCube(const FVector& center, const FVector4& color, const FMatrix& modelMatrix)
{
    FVector localVertices[8] = {
        { -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 }, { -1, 1, -1 },
        { -1, -1,  1 }, { 1, -1,  1 }, { 1, 1,  1 }, { -1, 1,  1 }
    };

    FVector worldVertices[8];
    worldVertices[0] = center + FMatrix::TransformVector(localVertices[0], modelMatrix);

    FVector min = worldVertices[0], max = worldVertices[0];

    // 첫 번째 값을 제외한 나머지 버텍스를 변환하고 min/max 계산
    for (int i = 1; i < 8; ++i)
    {
        worldVertices[i] = center + FMatrix::TransformVector(localVertices[i], modelMatrix);

        min.x = (worldVertices[i].x < min.x) ? worldVertices[i].x : min.x;
        min.y = (worldVertices[i].y < min.y) ? worldVertices[i].y : min.y;
        min.z = (worldVertices[i].z < min.z) ? worldVertices[i].z : min.z;

        max.x = (worldVertices[i].x > max.x) ? worldVertices[i].x : max.x;
        max.y = (worldVertices[i].y > max.y) ? worldVertices[i].y : max.y;
        max.z = (worldVertices[i].z > max.z) ? worldVertices[i].z : max.z;
    }

    AddBox(min, max, color);
}

void UPrimitiveBatch::AddBox(const FVector& minPoint, const FVector& maxPoint, const FVector4& color)
{
    // `FVector` 배열 없이 직접 `AddLine` 호출
    AddLine({ minPoint.x, minPoint.y, minPoint.z }, { maxPoint.x, minPoint.y, minPoint.z }, color);
    AddLine({ maxPoint.x, minPoint.y, minPoint.z }, { maxPoint.x, maxPoint.y, minPoint.z }, color);
    AddLine({ maxPoint.x, maxPoint.y, minPoint.z }, { minPoint.x, maxPoint.y, minPoint.z }, color);
    AddLine({ minPoint.x, maxPoint.y, minPoint.z }, { minPoint.x, minPoint.y, minPoint.z }, color);

    AddLine({ minPoint.x, minPoint.y, maxPoint.z }, { maxPoint.x, minPoint.y, maxPoint.z }, color);
    AddLine({ maxPoint.x, minPoint.y, maxPoint.z }, { maxPoint.x, maxPoint.y, maxPoint.z }, color);
    AddLine({ maxPoint.x, maxPoint.y, maxPoint.z }, { minPoint.x, maxPoint.y, maxPoint.z }, color);
    AddLine({ minPoint.x, maxPoint.y, maxPoint.z }, { minPoint.x, minPoint.y, maxPoint.z }, color);

    AddLine({ minPoint.x, minPoint.y, minPoint.z }, { minPoint.x, minPoint.y, maxPoint.z }, color);
    AddLine({ maxPoint.x, minPoint.y, minPoint.z }, { maxPoint.x, minPoint.y, maxPoint.z }, color);
    AddLine({ maxPoint.x, maxPoint.y, minPoint.z }, { maxPoint.x, maxPoint.y, maxPoint.z }, color);
    AddLine({ minPoint.x, maxPoint.y, minPoint.z }, { minPoint.x, maxPoint.y, maxPoint.z }, color);
}

void UPrimitiveBatch::AddBoxForSphere(const FVector& center,bool isUniform, float radius, const FMatrix& modelMatrix, const FVector4& color)
{
    FVector minPoint, maxPoint;
    float r = radius;
    if (isUniform)
    {
        // 균일 스케일: 반지름 그대로 사용
        minPoint = FVector(center.x - r, center.y - r, center.z - r);
        maxPoint = FVector(center.x + r, center.y + r, center.z + r);
    }
    else
    {
        FVector localOffsets[8] = {
            { -1, -1, -1 }, { 1, -1, -1 }, { -1,  1, -1 }, { 1,  1, -1 },
            { -1, -1,  1 }, { 1, -1,  1 }, { -1,  1,  1 }, { 1,  1,  1 }
        };

        minPoint = maxPoint = center  + FMatrix::TransformVector(localOffsets[0] * r, modelMatrix);

        for (int i = 1; i < 8; ++i)
        {
            FVector transformed = center + FMatrix::TransformVector(localOffsets[i] * r, modelMatrix);

            minPoint.x = (transformed.x < minPoint.x) ? transformed.x : minPoint.x;
            minPoint.y = (transformed.y < minPoint.y) ? transformed.y : minPoint.y;
            minPoint.z = (transformed.z < minPoint.z) ? transformed.z : minPoint.z;

            maxPoint.x = (transformed.x > maxPoint.x) ? transformed.x : maxPoint.x;
            maxPoint.y = (transformed.y > maxPoint.y) ? transformed.y : maxPoint.y;
            maxPoint.z = (transformed.z > maxPoint.z) ? transformed.z : maxPoint.z;
        }
    }

    AddBox(minPoint, maxPoint, color);
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

void UPrimitiveBatch::AddWorldGizmo()
{
    AddLine(FVector(0.0f, 0.0f, 0.01f), FVector(1000.0f, 0.0f, 0.01f), FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddLine(FVector(0.0f, 0.0f, 0.01f), FVector(0.0f, 1000.0f, 0.01f), FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AddLine(FVector(0.0f, 0.0f, 0.00f), FVector(0.0f, 0.0f, 1000.0f), FVector4(0.0f, 0.0f, 1.0f, 1.0f));

}
