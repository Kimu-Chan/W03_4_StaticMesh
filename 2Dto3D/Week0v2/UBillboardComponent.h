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
private:
	FMatrix CreateBillboardMatrix();
	void CreateQuadTextureVertexBuffer();
	UTexture m_texture;

	ID3D11Buffer* vertexTextureBuffer;
	ID3D11Buffer* indexTextureBuffer;
	UINT numVertices;
	UINT numIndices;

private:
	int CellsPerRow = 16;
	int CellsPerColumn = 16;
	int CellWidth = 32;
	int CellHeight = 32;
	int BitmapWidth = 512;
	int BitmapHeight = 512;

	float u = 0;
	float v = 0;

	void UpdateVertexBuffer(const TArray<FVertexTexture>& vertices);



};

