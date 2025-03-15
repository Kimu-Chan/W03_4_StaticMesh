#pragma once
#include "Define.h"
#include <Windows.h>
#include "IWindowToggleable.h"

#include <cstdint>

class UWorld;
class ShowFlags : public IWindowToggleable
{
private:
	ShowFlags();

public:
	~ShowFlags();

	static ShowFlags& GetInstance();

	void Draw(UWorld& world);
	void OnResize(HWND hWnd);
	void Toggle() override {
		if (bWasOpen) {
			bWasOpen = false;
		}
	}
private:
	bool bWasOpen = true;
	UINT width;
	UINT height;
};

enum class EEngineShowFlags : uint64
{
	SF_Primitives,
	SF_BillboardText,
};
