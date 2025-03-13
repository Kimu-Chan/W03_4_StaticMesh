#pragma once
#include "PrimitiveComponent.h"

class UDiscHollowComponent :
    public UPrimitiveComponent
{
public:
    UDiscHollowComponent(EPrimitiveColor color, float innerRadius =0.95f, FString type = "Disc");
    UDiscHollowComponent();
    ~UDiscHollowComponent();
    //virtual void Update();
    //virtual bool Intersects(const FVector& rayOrigin, const FVector& rayDir, float& dist);
    //virtual bool PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance) override;
    virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist);
    virtual	void		Render()					override;

    float inner;


};
