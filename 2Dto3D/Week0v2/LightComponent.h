#pragma once
#include "SceneComponent.h"
#include "Define.h"
class UBillboardComponent;
class ULightComponentBase :public USceneComponent
{
	DECLARE_CLASS(ULightComponentBase, USceneComponent)
public:
	ULightComponentBase();
	~ULightComponentBase();

	virtual void		Render();
	virtual void		Update(double deltaTim);
	virtual int			CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);

private:
	UBillboardComponent* texture2D;
	FBoundingBox AABB;
};

