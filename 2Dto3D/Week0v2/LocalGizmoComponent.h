#pragma once
#include "SceneComponent.h"
class LocalGizmoComponent : public USceneComponent
{
	DECLARE_CLASS(LocalGizmoComponent, USceneComponent)

public:
	LocalGizmoComponent();
	virtual				~LocalGizmoComponent();

	virtual void		Initialize();
	virtual void		Update(double deltaTime);
	virtual void		Release();
};

