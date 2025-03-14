#pragma once
#include "UBillboardComponent.h"
class UText :
    public UBillboardComponent
{
public:
	UText();
	~UText();
	virtual void		Initialize()				override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual void		Render()					override;
	void SetText(FWString _text);
	void SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn);
private:

	ID3D11Buffer* vertexTextBuffer;
	UINT numTextVertices;

	int RowCount;
	int ColumnCount;

	float quadWidth = 2;
	float quadHeight = 2;

	void setStartUV(char alphabet, float& outStartU, float& outStartV);
	void CreateTextTextureVertexBuffer(const TArray<FVertexTexture>& _vertex,UINT byteWidth);

};

