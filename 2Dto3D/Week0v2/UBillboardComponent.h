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
	void SetTexture(FWString _fileName);
protected:
	UTexture m_texture;
	FMatrix CreateBillboardMatrix();
	void CreateQuadTextureVertexBuffer();

	ID3D11Buffer* vertexTextureBuffer;
	ID3D11Buffer* indexTextureBuffer;
	UINT numVertices;
	UINT numIndices;

private:
	int CellsPerRow = 6;
	int CellsPerColumn = 6;
	float CellWidth = 341.3;
	float CellHeight = 341.3;
	int BitmapWidth = 2048;
	int BitmapHeight = 2048;

	float u = 0;
	float v = 0;

	void UpdateVertexBuffer(const TArray<FVertexTexture>& vertices);



};

