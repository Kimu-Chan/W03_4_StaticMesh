#pragma once
#include <cmath>
#include <algorithm>

// 기본 타입 
#include "CoreTypes.h"


// 수학 관련
#include "MathDefines.h"
#include "Vector.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Quat.h"

// 객체 매크로 정의 
#include "ObjectMacros.h"
#include "ObjectTypes.h"


#pragma once


#define UE_LOG Console::GetInstance().AddLog
#define Super __super


#include "Console.h"
#include "NameTypes.h"

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

struct FVertexTexture
{
	float x, y, z;    // Position
	float u, v; // Texture
};



