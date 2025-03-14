#pragma once
#include "UBillboardComponent.h"
class UParticleSubUVComp :
    public UBillboardComponent
{
public:
	UParticleSubUVComp();
	~UParticleSubUVComp();
	virtual void		Initialize()				override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual void		Render()					override;
private:

	int CellsPerRow = 6;
	int CellsPerColumn = 6;
	float CellWidth = 341.3;
	float CellHeight = 341.3;
	int BitmapWidth = 2048;
	int BitmapHeight = 2048;


	void UpdateVertexBuffer(const TArray<FVertexTexture>& vertices);

};

