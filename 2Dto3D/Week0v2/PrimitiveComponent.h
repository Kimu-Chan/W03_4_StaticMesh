#pragma once
#include "SceneComponent.h"
#include "ShowFlags.h"
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
	virtual int			CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
	bool				IntersectRayTriangle(const FVector& rayOrigin, const FVector& rayDirection, 
											 const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance);

private:
	FString m_Type;
	std::shared_ptr<FStaticMesh> staticMesh;
public:
	FString GetType() { return m_Type; }
	void	SetType(FString _Type) { 
		m_Type = _Type; 
		staticMesh = FEngineLoop::resourceMgr.GetMesh(m_Type);
	}
};