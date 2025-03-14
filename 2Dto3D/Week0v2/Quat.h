#pragma once

// 쿼터니언
struct FQuat
{
	float w, x, y, z;

	// 기본 생성자
	FQuat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

	// FQuat 생성자 추가: 회전 축과 각도를 받아서 FQuat 생성
	FQuat(const FVector& Axis, float Angle)
	{
		float HalfAngle = Angle * 0.5f;
		float SinHalfAngle = sinf(HalfAngle);
		float CosHalfAngle = cosf(HalfAngle);

		x = Axis.x * SinHalfAngle;
		y = Axis.y * SinHalfAngle;
		z = Axis.z * SinHalfAngle;
		w = CosHalfAngle;
	}

	// w, x, y, z 값으로 초기화
	FQuat(float InW, float InX, float InY, float InZ) : w(InW), x(InX), y(InY), z(InZ) {}

	// 쿼터니언의 곱셈 연산 (회전 결합)
	FQuat operator*(const FQuat& Other) const
	{
		return FQuat(
			w * Other.w - x * Other.x - y * Other.y - z * Other.z,
			w * Other.x + x * Other.w + y * Other.z - z * Other.y,
			w * Other.y - x * Other.z + y * Other.w + z * Other.x,
			w * Other.z + x * Other.y - y * Other.x + z * Other.w
		);
	}

	// (쿼터니언) 벡터 회전
	FVector RotateVector(const FVector& Vec) const
	{
		// 벡터를 쿼터니언으로 변환
		FQuat vecQuat(0.0f, Vec.x, Vec.y, Vec.z);
		// 회전 적용 (q * vec * q^-1)
		FQuat conjugate = FQuat(w, -x, -y, -z);  // 쿼터니언의 켤레
		FQuat result = *this * vecQuat * conjugate;

		return FVector(result.x, result.y, result.z); // 회전된 벡터 반환
	}

	// 단위 쿼터니언 여부 확인
	bool IsNormalized() const
	{
		return fabs(w * w + x * x + y * y + z * z - 1.0f) < 1e-6f;
	}

	// 쿼터니언 정규화 (단위 쿼터니언으로 만듬)
	FQuat Normalize() const
	{
		float magnitude = sqrtf(w * w + x * x + y * y + z * z);
		return FQuat(w / magnitude, x / magnitude, y / magnitude, z / magnitude);
	}

	// 회전 각도와 축으로부터 쿼터니언 생성 (axis-angle 방식)
	static FQuat FromAxisAngle(const FVector& Axis, float Angle)
	{
		float halfAngle = Angle * 0.5f;
		float sinHalfAngle = sinf(halfAngle);
		return FQuat(cosf(halfAngle), Axis.x * sinHalfAngle, Axis.y * sinHalfAngle, Axis.z * sinHalfAngle);
	}

	// 쿼터니언을 회전 행렬로 변환
	FMatrix ToMatrix() const
	{
		// 회전 행렬의 요소 계산
		FMatrix RotationMatrix;
		RotationMatrix.M[0][0] = 1.0f - 2.0f * (y * y + z * z);
		RotationMatrix.M[0][1] = 2.0f * (x * y - w * z);
		RotationMatrix.M[0][2] = 2.0f * (x * z + w * y);

		RotationMatrix.M[1][0] = 2.0f * (x * y + w * z);
		RotationMatrix.M[1][1] = 1.0f - 2.0f * (x * x + z * z);
		RotationMatrix.M[1][2] = 2.0f * (y * z - w * x);

		RotationMatrix.M[2][0] = 2.0f * (x * z - w * y);
		RotationMatrix.M[2][1] = 2.0f * (y * z + w * x);
		RotationMatrix.M[2][2] = 1.0f - 2.0f * (x * x + y * y);

		RotationMatrix.M[3][0] = RotationMatrix.M[3][1] = RotationMatrix.M[3][2] = 0.0f;
		RotationMatrix.M[3][3] = 1.0f;

		return RotationMatrix;
	}
};