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
	void				AddLocation(FVector _added);
	void				AddRotation(FVector _added);
	void				AddScale(FVector _added);

	void				AddChild(USceneComponent* _newChild);

	//void				SetAttachedUUID(UTextUUID* attachedUUID);
protected:
	FVector				RelativeLocation;
	FVector				RelativeRotation;
	FVector				RelativeScale3D;
	USceneComponent*	AttachParent = nullptr;
	TArray<USceneComponent*> AttachChildren;

public:
	FVector						GetWorldRotation();
	FVector						GetWorldScale();
	FVector						GetWorldLocation();
	inline FVector				GetLocalRotation() { return RelativeRotation; }
	inline FVector				GetLocalScale() { return RelativeScale3D; }
	inline FVector				GetLocalLocation() { return RelativeLocation; }

	inline void					SetLocation(FVector _newLoc) { RelativeLocation = _newLoc; }
	inline void					SetRotation(FVector _newRot) { RelativeRotation = _newRot; }
	inline void					SetScale(FVector _newScale) { RelativeScale3D = _newScale; }
	inline void					SetParent(USceneComponent* _parent) { AttachParent = _parent; }

private:

};

