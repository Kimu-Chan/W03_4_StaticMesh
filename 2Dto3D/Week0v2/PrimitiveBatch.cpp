#include "PrimitiveBatch.h"
#include "EngineLoop.h"
#include "EditorViewportClient.h"
extern FEngineLoop GEngineLoop;

UPrimitiveBatch::UPrimitiveBatch()
{

    GenerateGrid(5, 50000);
}

UPrimitiveBatch::~UPrimitiveBatch()
{
    if (pVertexBuffer) {
        pVertexBuffer->Release();
        pVertexBuffer = nullptr;
    }
    if (pBoundingBoxBuffer) {
        pBoundingBoxBuffer->Release();
        pBoundingBoxBuffer = nullptr;
    }
    if (pBoundingBoxSRV) {
        pBoundingBoxSRV->Release();
        pBoundingBoxSRV = nullptr;
    }
    if (pConesBuffer) {
        pConesBuffer->Release();
        pConesBuffer = nullptr;
    }
    if (pConesSRV) {
        pConesSRV->Release();
        pConesSRV = nullptr;
    }
}

void UPrimitiveBatch::GenerateGrid(float spacing, int gridCount)
{
    GridParam.gridSpacing = spacing;
    GridParam.numGridLines = gridCount;
    GridParam.gridOrigin = { 0,0,0 };
}

void UPrimitiveBatch::RenderBatch(const FMatrix& View, const FMatrix& Projection)
{
    FEngineLoop::renderer.PrepareLineShader();

    if (!pVertexBuffer)
        pVertexBuffer = FEngineLoop::renderer.CreateStaticVerticesBuffer();

    FMatrix Model = FMatrix::Identity;
    FMatrix MVP = Model * View * Projection;
    FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);
    FEngineLoop::renderer.UpdateGridConstantBuffer(GridParam);

    if (BoundingBoxes.size() > allocatedBoundingBoxCapacity) {
        allocatedBoundingBoxCapacity = BoundingBoxes.size();

        if (pBoundingBoxBuffer) pBoundingBoxBuffer->Release();
        if (pBoundingBoxSRV) pBoundingBoxSRV->Release();

        pBoundingBoxBuffer = FEngineLoop::renderer.CreateBoundingBoxBuffer(allocatedBoundingBoxCapacity);
        pBoundingBoxSRV = FEngineLoop::renderer.CreateBoundingBoxSRV(pBoundingBoxBuffer, allocatedBoundingBoxCapacity);
    }
    
    if (pBoundingBoxBuffer && pBoundingBoxSRV)
        FEngineLoop::renderer.UpdateBoundingBoxBuffer(pBoundingBoxBuffer, BoundingBoxes, BoundingBoxes.size());


    if (Cones.size() > allocatedConeCapacity) {
        allocatedConeCapacity = Cones.size();

        if (pConesBuffer) pConesBuffer->Release();
        if (pConesSRV) pConesSRV->Release();

        pConesBuffer = FEngineLoop::renderer.CreateConeBuffer(allocatedConeCapacity);
        pConesSRV = FEngineLoop::renderer.CreateConeSRV(pConesBuffer, allocatedConeCapacity);
    }
    if (pConesBuffer && pConesSRV)
        FEngineLoop::renderer.UpdateConesBuffer(pConesBuffer, Cones, Cones.size());



    FEngineLoop::renderer.UpdateLinePrimitveCountBuffer(BoundingBoxes.size(), Cones.size());
    FEngineLoop::renderer.RenderBatch(GridParam, pVertexBuffer, BoundingBoxes.size(), Cones.size(), ConeSegmentCount);
    BoundingBoxes.clear();
    Cones.clear();
}

void UPrimitiveBatch::AddBoxForCube(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix)
{
    FVector localVertices[8] = {
         { localAABB.min.x, localAABB.min.y, localAABB.min.z },
         { localAABB.max.x, localAABB.min.y, localAABB.min.z },
         { localAABB.min.x, localAABB.max.y, localAABB.min.z },
         { localAABB.max.x, localAABB.max.y, localAABB.min.z },
         { localAABB.min.x, localAABB.min.y, localAABB.max.z },
         { localAABB.max.x, localAABB.min.y, localAABB.max.z },
         { localAABB.min.x, localAABB.max.y, localAABB.max.z },
         { localAABB.max.x, localAABB.max.y, localAABB.max.z }
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
    FBoundingBox BoundingBox;
    BoundingBox.min = min;
    BoundingBox.max = max;
    BoundingBoxes.push_back(BoundingBox);
}
void UPrimitiveBatch::AddBoxForSphere(const FVector& center, bool isUniform, FVector radius, const FMatrix& modelMatrix)
{
    FVector minPoint, maxPoint;
    FVector r = radius;

    if (isUniform)
    {
        minPoint = center - r;
        maxPoint = center + r;
    }
    else
    {
        FMatrix AbsModelMatrix = modelMatrix;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                AbsModelMatrix.M[i][j] = fabs(AbsModelMatrix.M[i][j]);
            }
        }

         FVector transformedRadius = FVector(
            AbsModelMatrix.M[0][0] * r.x + AbsModelMatrix.M[1][0] * r.x + AbsModelMatrix.M[2][0] * r.x,
            AbsModelMatrix.M[0][1] * r.y + AbsModelMatrix.M[1][1] * r.y + AbsModelMatrix.M[2][1] * r.y,
            AbsModelMatrix.M[0][2] * r.z + AbsModelMatrix.M[1][2] * r.z + AbsModelMatrix.M[2][2] * r.z
        );


        FVector transformedCenter = modelMatrix.TransformPosition(center);

        minPoint = transformedCenter - transformedRadius;
        maxPoint = transformedCenter + transformedRadius;
    }

    FBoundingBox BoundingBox;
    BoundingBox.min = minPoint;
    BoundingBox.max = maxPoint;
    BoundingBoxes.push_back(BoundingBox);
}

void UPrimitiveBatch::AddCone(const FVector& center, float radius, float height, int segments, const FMatrix& modelMatrix)
{
    ConeSegmentCount = segments;
    FVector localApex = FVector(0, 0, 0);
    FCone cone;
    cone.ConeApex = center + FMatrix::TransformVector(localApex, modelMatrix);
    FVector localBaseCenter = FVector(0, 0, height);
    cone.ConeBaseCenter = center + FMatrix::TransformVector(localBaseCenter, modelMatrix);
    cone.ConeRadius = radius;
    cone.ConeHeight = height;
    cone.ConeSegmentCount = ConeSegmentCount;
    Cones.push_back(cone);
}

