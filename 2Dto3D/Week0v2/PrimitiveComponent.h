#pragma once
#include "SceneComponent.h"
class UPrimitiveComponent : public USceneComponent
{
	DECLARE_CLASS(UPrimitiveComponent, USceneComponent)
public:
	UPrimitiveComponent();
	UPrimitiveComponent(FString _Type);
	~UPrimitiveComponent();

	virtual void		Initialize()	override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual void		Render()					override;
private:
	FString m_Type;
public:
	FString GetType() { return m_Type; }
	void	SetType(FString _Type) { m_Type = _Type; }
};

