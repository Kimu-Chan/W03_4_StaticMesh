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
    if (pOBBSRV) {
        pOBBSRV->Release();
        pOBBSRV = nullptr;
    }  
    if (pOBBBuffer) {
        pOBBBuffer->Release();
        pOBBBuffer = nullptr;
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

    InitializeVertexBuffer();

    FMatrix Model = FMatrix::Identity;
    FMatrix MVP = Model * View * Projection;
    FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);
    FEngineLoop::renderer.UpdateGridConstantBuffer(GridParam);

    UpdateBoundingBoxResources();
    UpdateConeResources();
    UpdateOBBResources();

    FEngineLoop::renderer.UpdateLinePrimitveCountBuffer(BoundingBoxes.size(), Cones.size());
    FEngineLoop::renderer.RenderBatch(GridParam, pVertexBuffer, BoundingBoxes.size(), Cones.size(), ConeSegmentCount, OrientedBoundingBoxes.size());
    BoundingBoxes.clear();
    Cones.clear();
    OrientedBoundingBoxes.clear();
    FEngineLoop::renderer.PrepareShader();
}
void UPrimitiveBatch::InitializeVertexBuffer()
{
    if (!pVertexBuffer)
        pVertexBuffer = FEngineLoop::renderer.CreateStaticVerticesBuffer();
}

void UPrimitiveBatch::UpdateBoundingBoxResources()
{
    if (BoundingBoxes.size() > allocatedBoundingBoxCapacity) {
        allocatedBoundingBoxCapacity = BoundingBoxes.size();

        ReleaseBoundingBoxResources();

        pBoundingBoxBuffer = FEngineLoop::renderer.CreateBoundingBoxBuffer(allocatedBoundingBoxCapacity);
        pBoundingBoxSRV = FEngineLoop::renderer.CreateBoundingBoxSRV(pBoundingBoxBuffer, allocatedBoundingBoxCapacity);
    }

    if (pBoundingBoxBuffer && pBoundingBoxSRV)
        FEngineLoop::renderer.UpdateBoundingBoxBuffer(pBoundingBoxBuffer, BoundingBoxes, BoundingBoxes.size());
}

void UPrimitiveBatch::ReleaseBoundingBoxResources()
{
    if (pBoundingBoxBuffer) pBoundingBoxBuffer->Release();
    if (pBoundingBoxSRV) pBoundingBoxSRV->Release();
}

void UPrimitiveBatch::UpdateConeResources()
{
    if (Cones.size() > allocatedConeCapacity) {
        allocatedConeCapacity = Cones.size();

        ReleaseConeResources();

        pConesBuffer = FEngineLoop::renderer.CreateConeBuffer(allocatedConeCapacity);
        pConesSRV = FEngineLoop::renderer.CreateConeSRV(pConesBuffer, allocatedConeCapacity);
    }

    if (pConesBuffer && pConesSRV)
        FEngineLoop::renderer.UpdateConesBuffer(pConesBuffer, Cones, Cones.size());
}

void UPrimitiveBatch::ReleaseConeResources()
{
    if (pConesBuffer) pConesBuffer->Release();
    if (pConesSRV) pConesSRV->Release();
}

void UPrimitiveBatch::UpdateOBBResources()
{
    if (OrientedBoundingBoxes.size() > allocatedOBBCapacity) {
        allocatedOBBCapacity = OrientedBoundingBoxes.size();

        ReleaseOBBResources();

        pOBBBuffer = FEngineLoop::renderer.CreateOBBBuffer(allocatedOBBCapacity);
        pOBBSRV = FEngineLoop::renderer.CreateOBBSRV(pOBBBuffer, allocatedOBBCapacity);
    }

    if (pOBBBuffer && pOBBSRV)
        FEngineLoop::renderer.UpdateOBBBuffer(pOBBBuffer, OrientedBoundingBoxes, OrientedBoundingBoxes.size());
}
void UPrimitiveBatch::ReleaseOBBResources()
{
    if (pOBBBuffer) pOBBBuffer->Release();
    if (pOBBSRV) pOBBSRV->Release();
}
void UPrimitiveBatch::RenderAABB(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix)
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
void UPrimitiveBatch::RenderOBB(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix)
{
    // 1) 로컬 AABB의 8개 꼭짓점
    FVector localVertices[8] =
    {
        { localAABB.min.x, localAABB.min.y, localAABB.min.z },
        { localAABB.max.x, localAABB.min.y, localAABB.min.z },
        { localAABB.min.x, localAABB.max.y, localAABB.min.z },
        { localAABB.max.x, localAABB.max.y, localAABB.min.z },
        { localAABB.min.x, localAABB.min.y, localAABB.max.z },
        { localAABB.max.x, localAABB.min.y, localAABB.max.z },
        { localAABB.min.x, localAABB.max.y, localAABB.max.z },
        { localAABB.max.x, localAABB.max.y, localAABB.max.z }
    };

    FOBB faceBB;
    for (int32 i = 0; i < 8; ++i) {
        // 모델 매트릭스로 점을 변환 후, center를 더해준다.
        faceBB.corners[i] =  center + FMatrix::TransformVector(localVertices[i], modelMatrix);
    }

    OrientedBoundingBoxes.push_back(faceBB);

}

void UPrimitiveBatch::AddCone(const FVector& center, float radius, float height, int segments, const FVector4& color, const FMatrix& modelMatrix)
{
    ConeSegmentCount = segments;
    FVector localApex = FVector(0, 0, 0);
    FCone cone;
    cone.ConeApex = center + FMatrix::TransformVector(localApex, modelMatrix);
    FVector localBaseCenter = FVector(height, 0, 0);
    cone.ConeBaseCenter = center + FMatrix::TransformVector(localBaseCenter, modelMatrix);
    cone.ConeRadius = radius;
    cone.ConeHeight = height;
    cone.Color = color;
    cone.ConeSegmentCount = ConeSegmentCount;
    Cones.push_back(cone);
}

