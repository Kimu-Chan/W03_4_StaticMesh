#pragma once
#include<Windows.h>
class UWorld;

class PropertyPanel
{
private:
	PropertyPanel();

public:
	~PropertyPanel();
	static PropertyPanel& GetInstance()
	{
		static PropertyPanel Inst;
		return Inst;
	}
	void OnResize(HWND hWnd);
	void Draw(UWorld* world);
private:
	bool bWasOpen = true;
	UINT width;
	UINT height;
};