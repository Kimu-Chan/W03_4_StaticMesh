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
private:
	int RowCount = 16;
	int ColumnCount = 16;
	float CellWidth = 32;
	float CellHeight = 32;
	float BitmapWidth = 512;
	float BitmapHeight = 512;

	float quadWidth = 2;
	float quadHeight = 2;

	void setStartUV(char alphabet, float& outStartU, float& outStartV);


	void CreateTextTextureVertexBuffer(const TArray<FVertexTexture>& _vertex,UINT byteWidth);

};

