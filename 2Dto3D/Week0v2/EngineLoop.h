#pragma once
#include "Define.h"
#include <Windows.h>
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ResourceMgr.h"

class UImGuiManager;
class UWorld;
class FEngineLoop
{
public:
	FEngineLoop();
	~FEngineLoop();

	int32 PreInit();
	int32 Init(HINSTANCE hInstance);
	void Tick();
	void Exit();
	
private:
	void WindowInit(HINSTANCE hInstance);
	void Render();
public:
	static FGraphicsDevice graphicDevice;
	static FRenderer renderer;
	static FResourceMgr resourceMgr;
	static uint32 TotalAllocationBytes;
	static uint32 TotalAllocationCount;
	HWND hWnd;
	FMatrix View;
	FMatrix Projection;
private:
	UImGuiManager* UIMgr;
	UWorld* GWorld;
	bool bIsExit = false;
	const int32 targetFPS = 60;


public:
	UWorld* GetWorld(){ return GWorld; }
	
};

