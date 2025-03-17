#include "SphereComp.h"
#include "JungleMath.h"
#include "World.h"
#include "ShowFlags.h"

USphereComp::USphereComp() : UPrimitiveComponent("Sphere")
{
}

USphereComp::~USphereComp()
{
}

void USphereComp::Initialize()
{
	Super::Initialize();
}

void USphereComp::Update(double deltaTime)
{
}

void USphereComp::Release()
{
}

void USphereComp::Render()
{
	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetQuat(), GetWorldScale());

	// 최종 MVP 행렬
	FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	FEngineLoop::renderer.UpdateNormalConstantBuffer(Model);
	if (this == GetWorld()->GetPickingObj()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);

	FVector scale = GetWorldScale();
	float r = 1;
	bool isUniform = (fabs(scale.x - scale.y) < 1e-6f) && (fabs(scale.y - scale.z) < 1e-6f);
	r *= isUniform ? scale.x : 1;

	if (ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_AABB)) {
		UPrimitiveBatch::GetInstance().AddBoxForSphere(GetWorldLocation(), isUniform,r, Model,{ 1,1,1,1 });
		UPrimitiveBatch::GetInstance().AddCone(GetWorldLocation(), 3, 5, 140, { 1,1,1,1 }, Model);
	}

	if (ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
		Super::Render();
}
