#pragma once
#include "SceneComponent.h"
class UGizmoComponent : public USceneComponent
{
	DECLARE_CLASS(UGizmoComponent, USceneComponent)
public:
	UGizmoComponent();
	~UGizmoComponent();

	virtual void		Initialize()	override;
	virtual void		Update(double deltaTime)		override;
	virtual void		Release()					override;
	virtual	void		Render()					override;

};

