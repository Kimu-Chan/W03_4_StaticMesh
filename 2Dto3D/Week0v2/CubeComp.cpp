#include "CubeComp.h"
#include "JungleMath.h"
#include "World.h"
#include "PrimitiveBatch.h"
UCubeComp::UCubeComp() : UPrimitiveComponent("Cube")
{
}

UCubeComp::~UCubeComp()
{
}

void UCubeComp::Initialize()
{
	Super::Initialize();
}

void UCubeComp::Update(double deltaTime)
{
}

void UCubeComp::Release()
{
}

void UCubeComp::Render()
{
	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());

	// 최종 MVP 행렬
	FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	FEngineLoop::renderer.UpdateNormalConstantBuffer(Model);
	if (this == GetWorld()->GetPickingObj()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);

	UPrimitiveBatch::GetInstance().AddBox(GetWorldLocation(), { 1,1,1,1 }, Model);
	Super::Render();
}
