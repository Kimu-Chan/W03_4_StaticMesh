#include "JungleMath.h"
#include <DirectXMath.h>

using namespace DirectX;
FVector4 JungleMath::ConvertV3ToV4(FVector vec3)
{
	FVector4 newVec4;
	newVec4.x = vec3.x;
	newVec4.y = vec3.y;
	newVec4.z = vec3.z;
	return newVec4;
}



FMatrix JungleMath::CreateModelMatrix(FVector translation, FVector rotation, FVector scale)
{
    FMatrix Translation = FMatrix::CreateTranslationMatrix(translation);
    FMatrix Rotation = FMatrix::CreateRotation(rotation.x, rotation.y, rotation.z);
    FMatrix Scale = FMatrix::CreateScale(scale.x, scale.y, scale.z);
    return Scale * Rotation * Translation;
}

FMatrix JungleMath::CreateModelMatrix(FVector translation, FQuat rotation, FVector scale)
{
    FMatrix Translation = FMatrix::CreateTranslationMatrix(translation);
    //UE_LOG(LogLevel::Warning, "%f, %f, %f", rotation.x, rotation.y, rotation.z);
    FMatrix Rotation = rotation.ToMatrix();
    //UE_LOG(LogLevel::Warning, "%f, %f, %f %f", Rotation[0][0], Rotation[0][1], Rotation[0][2], Rotation[0][3]);
    //UE_LOG(LogLevel::Warning, "%f, %f, %f %f", Rotation[1][0], Rotation[1][1], Rotation[1][2], Rotation[1][3]);
    //UE_LOG(LogLevel::Warning, "%f, %f, %f %f", Rotation[2][0], Rotation[2][1], Rotation[2][2], Rotation[2][3]);
    //UE_LOG(LogLevel::Warning, "%f, %f, %f %f", Rotation[3][0], Rotation[3][1], Rotation[3][2], Rotation[3][3]);

    //Rotation = FMatrix::Identity;
    FMatrix Scale = FMatrix::CreateScale(scale.x, scale.y, scale.z);
    return Scale * Rotation * Translation;
}
FMatrix JungleMath::CreateViewMatrix(FVector eye, FVector target, FVector up)
{
    FVector zAxis = (target - eye).Normalize();  // DirectX는 LH이므로 -z가 아니라 +z 사용
    FVector xAxis = (up.Cross(zAxis)).Normalize();
    FVector yAxis = zAxis.Cross(xAxis);

    FMatrix View;
    View.M[0][0] = xAxis.x; View.M[0][1] = yAxis.x; View.M[0][2] = zAxis.x; View.M[0][3] = 0;
    View.M[1][0] = xAxis.y; View.M[1][1] = yAxis.y; View.M[1][2] = zAxis.y; View.M[1][3] = 0;
    View.M[2][0] = xAxis.z; View.M[2][1] = yAxis.z; View.M[2][2] = zAxis.z; View.M[2][3] = 0;
    View.M[3][0] = -xAxis.Dot(eye);
    View.M[3][1] = -yAxis.Dot(eye);
    View.M[3][2] = -zAxis.Dot(eye);
    View.M[3][3] = 1;

    return View;
}

FMatrix JungleMath::CreateProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane)
{
    float tanHalfFOV = tan(fov / 2.0f);
    float depth = farPlane - nearPlane;

    FMatrix Projection = {};
    Projection.M[0][0] = 1.0f / (aspect * tanHalfFOV);
    Projection.M[1][1] = 1.0f / tanHalfFOV;
    Projection.M[2][2] = farPlane / depth;
    Projection.M[2][3] = 1.0f;
    Projection.M[3][2] = -(nearPlane * farPlane) / depth;
    Projection.M[3][3] = 0.0f;  

    return Projection;
}

FVector JungleMath::FVectorRotate(FVector& origin, const FVector& rotation)
{
    // 회전 각도를 라디안으로 변환
    float pitch = DegToRad(rotation.x) * 0.5f;
    float yaw = DegToRad(rotation.y) * 0.5f;
    float roll = DegToRad(rotation.z) * 0.5f;

    // 쿼터니언 생성 (Yaw-Pitch-Roll 순서)
    float cosX = cosf(pitch), sinX = sinf(pitch);
    float cosY = cosf(yaw), sinY = sinf(yaw);
    float cosZ = cosf(roll), sinZ = sinf(roll);

    FQuat quaternion;
    quaternion.w = cosX * cosY * cosZ + sinX * sinY * sinZ;
    quaternion.x = sinX * cosY * cosZ - cosX * sinY * sinZ;
    quaternion.y = cosX * sinY * cosZ + sinX * cosY * sinZ;
    quaternion.z = cosX * cosY * sinZ - sinX * sinY * cosZ;

    // 쿼터니언을 이용해 벡터 회전 적용
    return quaternion.RotateVector(origin);
}

FVector JungleMath::FVectorRotate(FVector& origin, const FQuat& rotation)
{
    return rotation.RotateVector(origin);
}

FMatrix JungleMath::CreateRotationMatrix(FVector rotation)
{
    XMVECTOR quatX = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), DegToRad(rotation.x));
    XMVECTOR quatY = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), DegToRad(rotation.y));
    XMVECTOR quatZ = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), DegToRad(rotation.z));

    XMVECTOR rotationQuat = XMQuaternionMultiply(quatZ, XMQuaternionMultiply(quatY, quatX));
    rotationQuat = XMQuaternionNormalize(rotationQuat);  // 정규화 필수

    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);
    FMatrix result = FMatrix::Identity;  // 기본값 설정 (단위 행렬)

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.M[i][j] = rotationMatrix.r[i].m128_f32[j];  // XMMATRIX에서 FMatrix로 값 복사
        }
    }
    return result;
}


float JungleMath::RadToDeg(float radian)
{
    return radian * (180.0f / PI);
}

float JungleMath::DegToRad(float degree)
{
    return degree * (PI / 180.0f);
}


FQuat JungleMath::EulerToQuaternion(const FVector& eulerDegrees)
{
    float roll = DegToRad(eulerDegrees.x);  // X축 Roll
    float pitch = DegToRad(eulerDegrees.y); // Y축 Pitch
    float yaw = DegToRad(eulerDegrees.z);   // Z축 Yaw

    // 반각 계산
    float halfRoll = roll * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfYaw = yaw * 0.5f;

    // 코사인, 사인 값 미리 계산
    float cosRoll = cos(halfRoll);
    float sinRoll = sin(halfRoll);
    float cosPitch = cos(halfPitch);
    float sinPitch = sin(halfPitch);
    float cosYaw = cos(halfYaw);
    float sinYaw = sin(halfYaw);

    FQuat quat;
    // Roll-Pitch-Yaw(XYZ) 순서로 변환
    quat.w = cosRoll * cosPitch * cosYaw - sinRoll * sinPitch * sinYaw;
    quat.x = sinRoll * cosPitch * cosYaw + cosRoll * sinPitch * sinYaw;
    quat.y = cosRoll * sinPitch * cosYaw - sinRoll * cosPitch * sinYaw;
    quat.z = cosRoll * cosPitch * sinYaw + sinRoll * sinPitch * cosYaw;

    return quat;
}

FVector JungleMath::QuaternionToEuler(const FQuat& quat)
{
    FVector euler;

    // Roll (X 축 회전)
    float sinRoll = 2.0f * (quat.w * quat.x + quat.y * quat.z);
    float cosRoll = 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y);
    euler.x = RadToDeg(atan2(sinRoll, cosRoll));

    // Pitch (Y 축 회전)
    float sinPitch = 2.0f * (quat.w * quat.y - quat.z * quat.x);
    euler.y = RadToDeg(asin(sinPitch)); // Gimbal Lock 처리 없이 그대로 계산

    // Yaw (Z 축 회전)
    float sinYaw = 2.0f * (quat.w * quat.z + quat.x * quat.y);
    float cosYaw = 1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z);
    euler.z = RadToDeg(atan2(sinYaw, cosYaw));

    return euler;
}
//FQuat JungleMath::EulerToQuaternion(const FVector& eulerDegrees)
//{
//    float yaw = DegToRad(eulerDegrees.z);   // Z축 Yaw
//    float pitch = DegToRad(eulerDegrees.y); // Y축 Pitch
//    float roll = DegToRad(eulerDegrees.x);  // X축 Roll
//
//    float halfYaw = yaw * 0.5f;
//    float halfPitch = pitch * 0.5f;
//    float halfRoll = roll * 0.5f;
//
//    float cosYaw = cos(halfYaw);
//    float sinYaw = sin(halfYaw);
//    float cosPitch = cos(halfPitch);
//    float sinPitch = sin(halfPitch);
//    float cosRoll = cos(halfRoll);
//    float sinRoll = sin(halfRoll);
//
//    return FQuat(
//        sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw, // X
//        cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw, // Y
//        cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw, // Z
//        cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw  // W
//    );
//}
//
//
//FVector JungleMath::QuaternionToEuler(const FQuat& q)
//{
//    FVector euler;
//
//    // Pitch (Y축) 계산
//    float sinPitch = 2.0f * (q.w * q.y - q.z * q.x);
//
//    // 특이점(Gimbal Lock) 체크
//    if (fabs(sinPitch) >= 0.9999f) {
//        // Gimbal Lock 상태
//        euler.x = 0.0f; // Roll을 0으로 설정
//        euler.y = (sinPitch > 0) ? (PI / 2.0f) : (-PI / 2.0f); // ±90도
//        euler.z = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)); // Yaw
//    }
//    else {
//        // 일반적인 경우
//        euler.x = atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)); // Roll
//        euler.y = asin(sinPitch); // Pitch
//        euler.z = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)); // Yaw
//    }
//
//    // 라디안을 도(degree)로 변환
//    return FVector(RadToDeg(euler.x), RadToDeg(euler.y), RadToDeg(euler.z));
//}