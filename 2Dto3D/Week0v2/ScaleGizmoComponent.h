#pragma once
#include "PrimitiveComponent.h"
class UScaleGizmoComponent : public UPrimitiveComponent
{
	DECLARE_CLASS(UScaleGizmoComponent, UPrimitiveComponent)

public:
	UScaleGizmoComponent();
	~UScaleGizmoComponent();

	virtual void		Initialize()				override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual	void		Render()					override;

};

