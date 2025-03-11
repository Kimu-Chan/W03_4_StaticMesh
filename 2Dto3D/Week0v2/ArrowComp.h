#pragma once
#include "PrimitiveComponent.h"
class UArrowComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UArrowComp, UPrimitiveComponent)

public:
	UArrowComp();
	~UArrowComp();

	virtual void		Initialize()				override;
	virtual void		Update(double deltaTime)	override;
	virtual void		Release()					override;
	virtual	void		Render()					override;

private:
	ARROW_DIR Dir;
public:
	ARROW_DIR GetDir() {
		return Dir;
	};
	void SetDir(ARROW_DIR _Dir) { Dir = _Dir; }
};

