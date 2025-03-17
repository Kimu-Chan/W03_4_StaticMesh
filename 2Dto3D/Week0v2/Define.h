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
struct FGridParameters
{
	float gridSpacing;
	int   numGridLines;
	FVector gridOrigin;
	float pad;
};
struct FSimpleVertex
{
	float dummy; // 내용은 사용되지 않음
};
struct FBoundingBox
{
	FVector min; // Minimum extents
	float pad;
	FVector max; // Maximum extents
	float pad1;
	bool Intersect(const FVector& rayOrigin, const FVector& rayDir) {
        float tmin = -FLT_MAX;
        float tmax = FLT_MAX;
        const float epsilon = 1e-6f;

        // X축 처리
        if (fabs(rayDir.x) < epsilon) {
            // ray가 X축에 평행한 경우, 원점이 slab 안에 있는지 검사
            if (rayOrigin.x < min.x || rayOrigin.x > max.x)
                return false;
        }
        else {
            float t1 = (min.x - rayOrigin.x) / rayDir.x;
            float t2 = (max.x - rayOrigin.x) / rayDir.x;
            if (t1 > t2) std::swap(t1, t2);
            tmin = (tmin > t1) ? tmin : t1;
            tmax = (tmax < t2) ? tmax : t2;
            if (tmin > tmax)
                return false;
        }

        // Y축 처리
        if (fabs(rayDir.y) < epsilon) {
            if (rayOrigin.y< min.y || rayOrigin.y > max.y)
                return false;
        }
        else {
            float t1 = (min.y - rayOrigin.y) / rayDir.y;
            float t2 = (max.y - rayOrigin.y) / rayDir.y;
            if (t1 > t2) std::swap(t1, t2);
            tmin = (tmin > t1) ? tmin : t1;
            tmax = (tmax < t2) ? tmax : t2;
            if (tmin > tmax)
                return false;
        }

        // Z축 처리
        if (fabs(rayDir.z) < epsilon) {
            if (rayOrigin.z < min.z || rayOrigin.z > max.z)
                return false;
        }
        else {
            float t1 = (min.z - rayOrigin.z) / rayDir.z;
            float t2 = (max.z - rayOrigin.z) / rayDir.z;
            if (t1 > t2) std::swap(t1, t2);
            tmin = (tmin > t1)? tmin : t1;
            tmax = (tmax < t2)? tmax : t2;
            if (tmin > tmax)
                return false;
        }

        return true;
	}
};
struct FCone
{
	FVector ConeApex; // 원뿔의 꼭짓점
	float padding0;

	FVector ConeBaseCenter; // 원뿔 밑면 중심
	float padding1;

	float ConeRadius; // 원뿔 밑면 반지름
	float ConeHeight; // 원뿔 높이 (Apex와 BaseCenter 간 차이)
	int ConeSegmentCount; // 원뿔 밑면 분할 수
	float padding2;
};
struct FPrimitiveCounts 
{
	int BoundingBoxCount;
	int pad;
	int ConeCount; 
	int pad1;
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



