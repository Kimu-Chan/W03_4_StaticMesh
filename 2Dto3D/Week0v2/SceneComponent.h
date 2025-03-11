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
	FVector				GetForwardVector();
	FVector				GetRightVector();
	FVector				GetUpVector();
	FVector				GetLocation();
	void				SetLocation(FVector _newLoc);
	void				AddLocation(FVector _added);
	void				AddRotation(FVector _added);
	void				AddScale(FVector _added);

protected:
	FVector				RelativeLocation;
	FVector				RelativeRotation;
	FVector				RelativeScale3D;
public:
	inline FVector				GetRotation() { return RelativeRotation; }
	inline FVector				GetScale()    {	return RelativeScale3D;    }
	inline void					SetRotation(FVector _newRot) { RelativeRotation = _newRot; }
	inline void					SetScale(FVector _newScale) { RelativeScale3D = _newScale; }


};

