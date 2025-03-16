#pragma once
#include "PrimitiveComponent.h"
#include "UTexture.h"

class UBillboardComponent :
    public UPrimitiveComponent
{
	DECLARE_CLASS(UBillboardComponent, UPrimitiveComponent)
public:
	UBillboardComponent();
	UBillboardComponent(FString _Type);
	~UBillboardComponent();
	virtual void		Initialize()				override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual void		Render()					override;
	virtual int			CheckRayIntersection(FVector& rayOrigin, 
		FVector& rayDirection, float& pfNearHitDistance);

	void SetTexture(FWString _fileName);
protected:
	UTexture m_texture;
	FMatrix CreateBillboardMatrix();

	ID3D11Buffer* vertexTextureBuffer;
	ID3D11Buffer* indexTextureBuffer;
	UINT numVertices;
	UINT numIndices;

	float width;
	float height;

private:
	void CreateQuadTextureVertexBuffer();
	bool CheckPickingOnNDC();


};

