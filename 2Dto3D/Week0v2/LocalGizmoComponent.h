#pragma once
#include "SceneComponent.h"

class UArrowComp;
class UDiscHollowComponent;
class ULocalGizmoComponent : public USceneComponent
{
	DECLARE_CLASS(ULocalGizmoComponent, USceneComponent)

public:
	ULocalGizmoComponent();
	virtual				~ULocalGizmoComponent();

	virtual void		Initialize();
	virtual void		Update(double deltaTime);
	virtual void		Release();

	TArray<UArrowComp*>& GetArrowArr() { return ArrowArr; }
private:
	TArray<UArrowComp*> ArrowArr;
	TArray< UDiscHollowComponent*> HollowDiscArr;
};

