#pragma once
#include "SceneComponent.h"

class UArrowComp;
class UDiscHollowComponent;
class UScaleGizmoComponent;

class ULocalGizmoComponent : public USceneComponent
{
	DECLARE_CLASS(ULocalGizmoComponent, USceneComponent)

public:
	ULocalGizmoComponent();
	virtual				~ULocalGizmoComponent();

	virtual void		Initialize();
	virtual void		Update(double deltaTime);
	virtual void		Release();
	virtual void		Render();

	TArray<UArrowComp*>& GetArrowArr() { return ArrowArr; }
	TArray<UDiscHollowComponent*>& GetDiscArr() { return HollowDiscArr; }
	TArray<UScaleGizmoComponent*>& GetScaleArr() { return ScaleArr; }
private:
	TArray<UArrowComp*> ArrowArr;
	TArray<UDiscHollowComponent*> HollowDiscArr;
	TArray<UScaleGizmoComponent*> ScaleArr;
};

