#pragma once
#include "Define.h"
class JungleMath
{
public:
	static FVector4 ConvertV3ToV4(FVector vec3);
	static FMatrix CreateModelMatrix(FVector translation, FVector rotation, FVector scale);
	static FMatrix CreateViewMatrix(FVector eye, FVector target, FVector up);
	static FMatrix CreateProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane);
	static FVector FVectorRotate(FVector& origin, const FVector& rotation);
	
	static float   RadToDeg(float radian);
	static float DegToRad(float degree);
};

