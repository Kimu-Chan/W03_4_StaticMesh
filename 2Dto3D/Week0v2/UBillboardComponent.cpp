#include "UBillboardComponent.h"
#include "JungleMath.h"
#include "Player.h"
UBillboardComponent::UBillboardComponent() : UPrimitiveComponent("Quad")
{
}

UBillboardComponent::UBillboardComponent(FString _Type) : UPrimitiveComponent(_Type)
{
}

UBillboardComponent::~UBillboardComponent()
{
}

void UBillboardComponent::Initialize()
{
    Super::Initialize();
}

void UBillboardComponent::Update(double deltaTime)
{
    Super::Update(deltaTime);
}

void UBillboardComponent::Release()
{
}

void UBillboardComponent::Render()
{
	if (!GetWorld()->GetPickingObj() || GetWorld()->GetPlayer()->GetControlMode() != CM_TRANSLATION)
		return;
	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());

	FMatrix CameraView = GetEngine().View;
	
	CameraView.M[0][3] = 0.0f;
	CameraView.M[1][3] = 0.0f;
	CameraView.M[2][3] = 0.0f;

	CameraView.M[3][0] = 0.0f;
	CameraView.M[3][1] = 0.0f;
	CameraView.M[3][2] = 0.0f;
	CameraView.M[3][3] = 1.0f;

	CameraView.M[0][2] = -CameraView.M[0][2];
	CameraView.M[1][2] = -CameraView.M[1][2];
	CameraView.M[2][2] = -CameraView.M[2][2];
	FMatrix LookAtCamera = CameraView;
	


	FMatrix S = FMatrix::CreateScale(RelativeScale3D.x, RelativeScale3D.x, RelativeScale3D.x);
	FMatrix R = CameraView;
	FMatrix T = FMatrix::CreateTranslationMatrix(RelativeLocation);
	FMatrix M = S * R * T;
	FMatrix VP = GetEngine().View * GetEngine().Projection;
	// 최종 MVP 행렬
	//FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	FMatrix MVP = M * VP;
	if (this == GetWorld()->GetPickingGizmo()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);

	Super::Render();
}
