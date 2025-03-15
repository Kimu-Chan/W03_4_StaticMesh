#pragma once
#include "Define.h"

class FEditorViewportClient
{
public:
	FEditorViewportClient();
	~FEditorViewportClient();
protected:
	/** Camera speed setting */
	int32 CameraSpeedSetting = 1;
	/** Camera speed scalar */
	float CameraSpeedScalar = 1.0f;
	float GridSize;

public:
	PROPERTY(int32, CameraSpeedSetting)//
	PROPERTY(float, CameraSpeedScalar)
	PROPERTY(float, GridSize)

};

