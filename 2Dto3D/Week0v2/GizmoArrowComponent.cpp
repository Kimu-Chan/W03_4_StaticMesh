#include "GizmoArrowComponent.h"
#include "JungleMath.h"
#include "Player.h"
UGizmoArrowComponent::UGizmoArrowComponent()
{
}

UGizmoArrowComponent::~UGizmoArrowComponent()
{
}

void UGizmoArrowComponent::Initialize()
{
	Super::Initialize();
}

void UGizmoArrowComponent::Update(double deltaTime)
{
	Super::Update(deltaTime);
}

void UGizmoArrowComponent::Release()
{
}

void UGizmoArrowComponent::Render()
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

	if (!GetWorld()->GetPickingObj() || GetWorld()->GetPlayer()->GetControlMode() != CM_TRANSLATION)
		return;
	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());

	// 최종 MVP 행렬
	FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	
	
	if (this == GetWorld()->GetPickingGizmo()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);

	FEngineLoop::graphicDevice.DeviceContext->RSSetState(FEngineLoop::graphicDevice.RasterizerStateSOLID); // fill solid로 렌더링.
	Super::Render();
	FEngineLoop::graphicDevice.DeviceContext->RSSetState(FEngineLoop::graphicDevice.GetCurrentRasterizer()); // 이전 레스터라이저 재설정.

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
