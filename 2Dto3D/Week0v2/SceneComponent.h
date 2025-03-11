#pragma once
#include "Object.h"
class USceneComponent : public UObject
{
	DECLARE_CLASS(USceneComponent, UObject)
public:
	USceneComponent();
	~USceneComponent();

	virtual void		Initialize()	override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual void		Render()					override;
public:
	FVector RelativeLocation;
	FVector RelativeRotation;
	FVector RelativeScale3D;


};

