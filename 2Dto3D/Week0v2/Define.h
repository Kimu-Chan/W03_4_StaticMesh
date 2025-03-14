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
	float nx, ny, nz;
};

struct FVertexTexture
{
	float x, y, z;    // Position
	float u, v; // Texture
};
struct FLighting
{
	float lightDirX, lightDirY, lightDirZ; // 조명 방향
	float pad1;                      // 16바이트 정렬용 패딩
	float lightColorX, lightColorY, lightColorZ;    // 조명 색상
	float pad2;                      // 16바이트 정렬용 패딩
	float AmbientFactor;             // ambient 계수
	float pad3; // 16바이트 정렬 맞춤 추가 패딩
	float pad4; // 16바이트 정렬 맞춤 추가 패딩
	float pad5; // 16바이트 정렬 맞춤 추가 패딩
};



