#pragma once
#include "Object.h"
class UArrowComp;
class USceneComponent;
class UPrimitiveComponent;
class UPlayer : public UObject
{
	DECLARE_CLASS(LocalGizmoComponent, UObject)

public:
	UPlayer();
	virtual				~UPlayer();

	virtual void		Initialize();
	virtual void		Update(double deltaTime);
	virtual void		Release();

	void				Input();
	void PickGizmo(FVector& rayOrigin);
	void PickObj(FVector& pickPosition);
	void				AddMode();

private:
	void ScreenToNDC(int screenX, int screenY, const FMatrix& viewMatrix, const FMatrix& projectionMatrix, FVector& rayOrigin);
	int RayIntersectsObject(const FVector& pickPosition, UPrimitiveComponent* obj, float& hitDistance, int& intersectCount);
	void PickedObjControl();
	void ControlRoation(UArrowComp* Arrow, USceneComponent* pObj, int32 deltaX, int32 deltaY);
	void ControlTranslation(USceneComponent* pObj, UArrowComp* Arrow, int32 deltaX, int32 deltaY);
	void ControlScale(USceneComponent* pObj, UArrowComp* Arrow, int32 deltaX, int32 deltaY);
	bool bLeftMouseDown = false;
	bool bSpaceDown = false;

	POINT m_LastMousePos;
	ControlMode cMode = CM_TRANSLATION;

public:
	void SetMode(ControlMode _Mode) { cMode = _Mode; }
	ControlMode GetMode() { return cMode; }
};

