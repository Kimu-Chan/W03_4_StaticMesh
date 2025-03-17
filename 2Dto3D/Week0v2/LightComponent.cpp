#include "LightComponent.h"
#include "UBillboardComponent.h"
#include "JungleMath.h"
#include "PrimitiveBatch.h"

ULightComponentBase::ULightComponentBase()
{
	AABB.max = { 1,1,1 };
	AABB.min = { -1,-1,-1 };
	texture2D = new UBillboardComponent();
	texture2D->SetTexture(L"Assets/Texture/spotLight.png");
	texture2D->Initialize();

}

ULightComponentBase::~ULightComponentBase()
{
	delete texture2D;
}

void ULightComponentBase::Render()
{
	texture2D->Render();
	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());
	//FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	UPrimitiveBatch::GetInstance().AddCone(GetWorldLocation(), 5, 15, 140, Model);
	UPrimitiveBatch::GetInstance().AddPlaneForCube(AABB, GetWorldLocation(), Model);
}

void ULightComponentBase::Update(double deltaTime)
{
	texture2D->Update(deltaTime);
	texture2D->SetLocation(GetWorldLocation());

}

int ULightComponentBase::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
	 bool res =AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance);
	 if (res)
		 UE_LOG(LogLevel::Error, "T!!!");
	 else 
		 UE_LOG(LogLevel::Error, "F!!!");

	 return res;
}

