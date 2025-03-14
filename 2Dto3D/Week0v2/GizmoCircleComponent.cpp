#include "GizmoCircleComponent.h"
#include "JungleMath.h"
#include "Player.h"
#define DISC_RESOLUTION 128

static const FVector Colors[] = {
    FVector(1.0f, 0.0f, 0.0f),  // X��: ����
    FVector(0.0f, 1.0f, 0.0f),  // Y��: �ʷ�
    FVector(0.0f, 0.0f, 1.0f)   // Z��: �Ķ�
};

UGizmoCircleComponent::UGizmoCircleComponent(EPrimitiveColor color, float innerRadius, FString type) : inner(innerRadius)
{
    inner = innerRadius;
    FVector color3 = Colors[color];
    TArray<FVertexSimple> vertices;
    TArray<uint32>  indices;
    if (vertices.empty())
    {
        float angleStep = 2.0f * 3.1415926535f / DISC_RESOLUTION;

        // 원의 외곽 버텍스 추가
        for (int i = 0; i <= DISC_RESOLUTION; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            vertices.push_back({ x, 0.0f, z ,color3.x, color3.y, color3.z, 1 });

            x *= innerRadius;
            z *= innerRadius;
            vertices.push_back({ x, 0.0f, z ,color3.x, color3.y, color3.z, 1 });
        }

        // 인덱스 설정 (삼각형 리스트)
        for (int i = 0; i <= DISC_RESOLUTION + 1; ++i) {
            int pivot = 2 * i;
            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 3);      // 현재 점
            indices.push_back(pivot + 2);  // 다음 점 (마지막 점 예외 처리)

            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 1);      // 현재 점
            indices.push_back(pivot + 3);  // 다음 점 (마지막 점 예외 처리)
        }
        for (int i = 0; i <= DISC_RESOLUTION + 1; ++i) {
            int pivot = 2 * i;
            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 2);      // 현재 점
            indices.push_back(pivot + 3);  // 다음 점 (마지막 점 예외 처리)

            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 3);      // 현재 점
            indices.push_back(pivot + 1);  // 다음 점 (마지막 점 예외 처리)
        }

    }

    FEngineLoop::resourceMgr.RegisterMesh(&FEngineLoop::renderer,
        type, vertices, vertices.size(), indices, indices.size());

}

UGizmoCircleComponent::UGizmoCircleComponent()
{
    inner = 0.5;
    TArray<FVertexSimple> vertices;
    TArray<uint32>  indices;

    if (vertices.empty())
    {
        float angleStep = 2.0f * 3.1415926535f / DISC_RESOLUTION;

        // 원의 외곽 버텍스 추가
        for (int i = 0; i <= DISC_RESOLUTION; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            vertices.push_back({ x, 0.0f, z ,1,1,1, 1 });

            x *= 0.5;
            z *= 0.5;
            vertices.push_back({ x, 0.0f, z ,1,1,1, 1 });
        }

        // 인덱스 설정 (삼각형 리스트)
        for (int i = 0; i <= DISC_RESOLUTION + 1; ++i) {
            int pivot = 2 * i;
            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 3);      // 현재 점
            indices.push_back(pivot + 2);  // 다음 점 (마지막 점 예외 처리)

            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 1);      // 현재 점
            indices.push_back(pivot + 3);  // 다음 점 (마지막 점 예외 처리)

        }
    }

    FEngineLoop::resourceMgr.RegisterMesh(&FEngineLoop::renderer,
        FString("Disc"), vertices, vertices.size(), indices, indices.size());

}

UGizmoCircleComponent::~UGizmoCircleComponent()
{
}

bool UGizmoCircleComponent::IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist)
{
    if (rayDir.y == 0) return false; // normal to normal vector of plane

    dist = -rayOrigin.y / rayDir.y;

    FVector intersectionPoint = rayOrigin + rayDir * dist;
    float intersectionToDiscCenterSquared = intersectionPoint.Magnitude();

    return (inner * inner < intersectionToDiscCenterSquared && intersectionToDiscCenterSquared < 1);
}

void UGizmoCircleComponent::Render()
{
#pragma region GizmoDepth
    static ID3D11DepthStencilState* gizmoDepthState = nullptr;

    if (gizmoDepthState == nullptr)
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = FALSE;  // 깊이 테스트 유지
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // 깊이 버퍼에 쓰지 않음
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;  // 깊이 비교를 항상 통과

        HRESULT hr = FEngineLoop::graphicDevice.Device->CreateDepthStencilState(&depthStencilDesc, &gizmoDepthState);
        if (FAILED(hr))
        {
            Console::GetInstance().AddLog(LogLevel::Error, "Failed to create DepthStencilState! HRESULT: ");
            return;
        }
    }

    // 기즈모 렌더링 전에 적용
    FEngineLoop::graphicDevice.DeviceContext->OMSetDepthStencilState(gizmoDepthState, 0);
#pragma endregion GizmoDepth

    if (!GetWorld()->GetPickingObj() || GetWorld()->GetPlayer()->GetControlMode() != CM_ROTATION)
        return;
    FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());

    // 최종 MVP 행렬
    FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
    if (this == GetWorld()->GetPickingGizmo()) {
        FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
    }
    else
        FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);

    Super::Render();

#pragma region GizmoDepth
    ID3D11DepthStencilState* currentState = nullptr;
    UINT stencilRef;
    FEngineLoop::graphicDevice.DeviceContext->OMGetDepthStencilState(&currentState, &stencilRef);
    if (currentState != gizmoDepthState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "DepthStencilState was overridden before rendering gizmo!");
    }
    ID3D11DepthStencilState* originalDepthState = FEngineLoop::graphicDevice.DepthStencilState;
    FEngineLoop::graphicDevice.DeviceContext->OMSetDepthStencilState(originalDepthState, 0);
#pragma endregion GizmoDepth
}

