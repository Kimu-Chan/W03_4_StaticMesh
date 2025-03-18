#pragma once
#include "PrimitiveComponent.h"
#include "Define.h"
class UBillboardComponent;
class ULightComponentBase :public UPrimitiveComponent
{
	DECLARE_CLASS(ULightComponentBase, UPrimitiveComponent)
public:
	ULightComponentBase();
	ULightComponentBase(FString m_type);
	~ULightComponentBase();

	virtual void		Render();
	virtual void		Update(double deltaTim);
	virtual int			CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
	void				InitializeLight();
	void				SetColor(FVector4 newColor);
	FVector4			GetColor();
	float				GetRadius();
	void				SetRadius(float r);
private:
	FVector4	color;
	float		radius;
	UBillboardComponent* texture2D;
};

