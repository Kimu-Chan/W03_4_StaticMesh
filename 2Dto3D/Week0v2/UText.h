#pragma once
#include "UBillboardComponent.h"
class UText :
    public UBillboardComponent
{
	UText();
	~UText();
	virtual void		Initialize()				override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual void		Render()					override;

};

