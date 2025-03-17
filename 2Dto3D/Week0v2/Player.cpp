#include "Player.h"
#include <Windows.h>
#include "GraphicDevice.h"
#include "World.h"
#include "Define.h"
#include "ShowFlags.h"
#include "EngineLoop.h"
#include "PrimitiveComponent.h"
#include "JungleMath.h"
#include <DirectXMath.h>
#include "GizmoArrowComponent.h"
#include "CameraComponent.h"
#include "TransformGizmo.h"
#include "GizmoCircleComponent.h"
#include "GizmoRectangleComponent.h"
using namespace DirectX;

UPlayer::UPlayer()
{
}

UPlayer::~UPlayer()
{
}

void UPlayer::Initialize()
{
	Super::Initialize();
}

void UPlayer::Update(double deltaTime)
{
	Input();
}

void UPlayer::Release()
{
}

void UPlayer::Input()
{

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 )
	{
		if (!bLeftMouseDown) {
			bLeftMouseDown = true;

			POINT mousePos;
			GetCursorPos(&mousePos);
			GetCursorPos(&m_LastMousePos);


			ScreenToClient(GEngineLoop.hWnd, &mousePos);

			FVector pickPosition;
		
			ScreenToNDC(mousePos.x, mousePos.y, GEngineLoop.View, GEngineLoop.Projection, pickPosition);
			PickObj(pickPosition);
			PickGizmo(pickPosition);
		}
		else
		{
			PickedObjControl();
		}
	}
	else
	{
		if (bLeftMouseDown) {
				bLeftMouseDown = false; // 마우스 오른쪽 버튼을 떼면 상태 초기화
				GetWorld()->SetPickingGizmo(nullptr);
		}
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!bSpaceDown) {
			AddControlMode();
			bSpaceDown = true;
		}
	}
	else
	{
		if (bSpaceDown)
		{
			bSpaceDown = false;
		}
	}
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeletePickedObj();
	}
}

void UPlayer::PickGizmo(FVector& pickPosition)
{
	if (GetWorld()->GetPickingObj()) {
		if (cMode == CM_TRANSLATION) {
			for (auto iter : GetWorld()->LocalGizmo->GetArrowArr())
			{
				int maxIntersect = 0;
				float minDistance = FLT_MAX;
				float Distance = 0.0f;
				int currentIntersectCount = 0;
				if (!iter) continue;
				if (RayIntersectsObject(pickPosition, iter, Distance, currentIntersectCount))
				{
					if (currentIntersectCount > maxIntersect && minDistance > Distance)
					{
						GetWorld()->SetPickingGizmo(iter);
						minDistance = Distance;
						maxIntersect = currentIntersectCount;
					}
				}
			}
		}
		else if (cMode == CM_ROTATION) {
			for (auto iter : GetWorld()->LocalGizmo->GetDiscArr())
			{
				int maxIntersect = 0;
				float minDistance = FLT_MAX;
				float Distance = 0.0f;
				int currentIntersectCount = 0;
				//UPrimitiveComponent* localGizmo = dynamic_cast<UPrimitiveComponent*>(GetWorld()->LocalGizmo[i]);
				if (!iter) continue;
				if (RayIntersectsObject(pickPosition, iter, Distance, currentIntersectCount))
				{
					if (currentIntersectCount > maxIntersect && minDistance > Distance)
					{
						GetWorld()->SetPickingGizmo(iter);
						minDistance = Distance;
						maxIntersect = currentIntersectCount;
					}
				}
			}
		}
		else if (cMode == CM_SCALE) {
			for (auto iter : GetWorld()->LocalGizmo->GetScaleArr())
			{
				int maxIntersect = 0;
				float minDistance = FLT_MAX;
				float Distance = 0.0f;
				int currentIntersectCount = 0;
				if (!iter) continue;
				if (RayIntersectsObject(pickPosition, iter, Distance, currentIntersectCount))
				{
					if (currentIntersectCount > maxIntersect && minDistance > Distance)
					{
						GetWorld()->SetPickingGizmo(iter);
						minDistance = Distance;
						maxIntersect = currentIntersectCount;
					}
				}
			}
		}
	}
}
void UPlayer::PickObj(FVector& pickPosition)
{
	if (!(ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_Primitives))) return;

	UObject* Possible = nullptr;
	int maxIntersect = 0;
			float minDistance = FLT_MAX;
	for (auto iter : GetWorld()->GetObjectArr())
	{
		UPrimitiveComponent* pObj = nullptr;
		if (iter->IsA(UPrimitiveComponent::StaticClass())) {
			pObj = static_cast<UPrimitiveComponent*>(iter);
		}
		else
			continue;
		if (pObj && !pObj->IsA(UGizmoBaseComponent::StaticClass()))
		{
			float Distance = 0.0f;
			int currentIntersectCount = 0;
			if (RayIntersectsObject(pickPosition, pObj, Distance, currentIntersectCount))
			{
				if (minDistance > Distance) {
					minDistance = Distance;
					if (currentIntersectCount > 0) {
						maxIntersect = currentIntersectCount;
						Possible = pObj;

					}
				}
			}
		}
	}
	if (Possible) {
		GetWorld()->SetPickingObj(Possible);
	}
}

void UPlayer::AddControlMode()
{
	cMode = static_cast<ControlMode>((cMode + 1) % ControlMode::CM_END);
}

void UPlayer::AddCoordiMode()
{
	cdMode = static_cast<CoordiMode>((cdMode + 1) % CoordiMode::CDM_END);
}

void UPlayer::DeletePickedObj()
{
	GetWorld()->ThrowAwayObj(GetWorld()->GetPickingObj());
	GetWorld()->SetPickingObj(nullptr);
}

void UPlayer::ScreenToNDC(int screenX, int screenY, const FMatrix& viewMatrix, const FMatrix& projectionMatrix, FVector& pickPosition)
{
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	FEngineLoop::graphicDevice.DeviceContext->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	pickPosition.x = ((2.0f * screenX / viewport.Width) - 1) / projectionMatrix[0][0];
	pickPosition.y = -((2.0f * screenY / viewport.Height) - 1) / projectionMatrix[1][1];
	pickPosition.z = 1.0f; // Near Plane
}
int UPlayer::RayIntersectsObject(const FVector& pickPosition, UPrimitiveComponent* obj, float& hitDistance, int& intersectCount)
{

	// 오브젝트의 월드 변환 행렬 생성 (위치, 회전, 크기 적용)
	FMatrix scaleMatrix = FMatrix::CreateScale(
		obj->GetWorldScale().x,
		obj->GetWorldScale().y,
		obj->GetWorldScale().z
	);

	//FMatrix rotationMatrix = JungleMath::CreateRotationMatrix(obj->GetWorldRotation());
		FMatrix rotationMatrix =FMatrix::CreateRotation(
		obj->GetWorldRotation().x,
		obj->GetWorldRotation().y,
		obj->GetWorldRotation().z
	);

	FMatrix translationMatrix = FMatrix::CreateTranslationMatrix(obj->GetWorldLocation());

	// 최종 변환 행렬
	FMatrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	FMatrix ViewMatrix = GEngineLoop.View;
	FMatrix inverseMatrix = FMatrix::Inverse(worldMatrix * ViewMatrix);

	FVector cameraOrigin = { 0,0,0 };

	FVector pickRayOrigin = inverseMatrix.TransformPosition(cameraOrigin);
	FVector rayDirection = inverseMatrix.TransformPosition(pickPosition);
	rayDirection = (rayDirection - pickRayOrigin).Normalize(); // local 좌표축의 ray
	intersectCount = obj->CheckRayIntersection(pickRayOrigin, rayDirection, hitDistance);
	return intersectCount;
}

void UPlayer::PickedObjControl()
{
	// 마우스 이동량 계산
	if (GetWorld()->GetPickingObj() && GetWorld()->GetPickingGizmo()) {
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);
		// 마우스 이동 차이 계산
		int32 deltaX = currentMousePos.x - m_LastMousePos.x;
		int32 deltaY = currentMousePos.y - m_LastMousePos.y;

		USceneComponent* pObj = GetWorld()->GetPickingObj();
		UPrimitiveComponent* Gizmo = static_cast<UPrimitiveComponent*>(GetWorld()->GetPickingGizmo());
		switch (cMode)
		{
		case CM_TRANSLATION:
			ControlTranslation(pObj, Gizmo, deltaX, deltaY);
			break;
		case CM_SCALE:
			ControlScale(pObj, Gizmo, deltaX, deltaY);

			break;
		case CM_ROTATION:
			ControlRotation(pObj, Gizmo, deltaX, deltaY);
			break;
		}
		// 새로운 마우스 위치 저장
		m_LastMousePos = currentMousePos;
	}
}

void UPlayer::ControlRotation(USceneComponent* pObj, UPrimitiveComponent* Gizmo, int32 deltaX, int32 deltaY)
{
		//if (cdMode == CDM_LOCAL) {
		//	FVector cameraForward = GetWorld()->GetCamera()->GetForwardVector();
		//	FVector cameraRight = GetWorld()->GetCamera()->GetRightVector();
		//	FVector cameraUp = GetWorld()->GetCamera()->GetUpVector();

		//	if (Gizmo->GetType() == "CircleX")
		//	{
		//		// X 축 회전 (카메라 방향에 따라 Y 축 이동량 반영)
		//		float rotationAmount = (cameraUp.z >= 0 ? 1.0f : -1.0f) * deltaY;

		//		pObj->AddRotation(FVector(-rotationAmount, 0.0f, 0.0f));
		//	}
		//	else if (Gizmo->GetType() == "CircleY")
		//	{
		//		// Y 축 회전 (카메라 방향에 따라 X 축 이동량 반영)
		//		float rotationAmount = (cameraRight.x >= 0 ? -1.0f : 1.0f) * deltaX;
		//		pObj->AddRotation(FVector(0.0f, -rotationAmount, 0.0f));
		//	}
		//	else if (Gizmo->GetType() == "CircleZ")
		//	{
		//		// Z 축 회전 (카메라 방향에 따라 X, Y 이동량 반영)
		//		float rotationAmount = (cameraForward.x <= 0 ? 1.0f : -1.0f) * deltaX;
		//		pObj->AddRotation(FVector(0.0f, 0.0f, rotationAmount));
		//	}

		//}
		if (cdMode == CDM_LOCAL) {
			FVector cameraForward = GetWorld()->GetCamera()->GetForwardVector();
			FVector cameraRight = GetWorld()->GetCamera()->GetRightVector();
			FVector cameraUp = GetWorld()->GetCamera()->GetUpVector();

			// 현재 로컬 회전값 가져오기
			FQuat currentRotation = pObj->GetQuat();

			FQuat rotationDelta;
			if (Gizmo->GetType() == "CircleX") {
				float rotationAmount = (cameraUp.z >= 0 ? 1.0f : -1.0f) * deltaY * 0.01f;
				rotationDelta = FQuat(pObj->GetForwardVector(), rotationAmount);
			}
			else if (Gizmo->GetType() == "CircleY") {
				float rotationAmount = (cameraRight.x >= 0 ? -1.0f : 1.0f) * deltaX * 0.01f;
				rotationDelta = FQuat(pObj->GetRightVector(), (rotationAmount));
			}
			else if (Gizmo->GetType() == "CircleZ") {
				float rotationAmount = (cameraForward.x <= 0 ? 1.0f : -1.0f) * deltaX * 0.01f;
				rotationDelta = FQuat(pObj->GetUpVector(), rotationAmount);
			}
			FQuat newRotation = currentRotation * rotationDelta;


			pObj->SetRotation(newRotation);
		}
		else if (cdMode == CDM_WORLD) {
			FVector cameraForward = GetWorld()->GetCamera()->GetForwardVector();
			FVector cameraRight = GetWorld()->GetCamera()->GetRightVector();
			FVector cameraUp = GetWorld()->GetCamera()->GetUpVector();

			// 현재 로컬 회전값 가져오기
			FQuat currentRotation = pObj->GetQuat();

			FQuat rotationDelta;

			if (Gizmo->GetType() == "CircleX") {
				float rotationAmount = (cameraUp.z >= 0 ? 1.0f : -1.0f) * deltaY * 0.01f;
				rotationDelta = FQuat(FVector(1.0f,0.0f,0.0f), rotationAmount); // 로컬 X 축 기준 회전
			}
			else if (Gizmo->GetType() == "CircleY") {
				float rotationAmount = (cameraRight.x >= 0 ? -1.0f : 1.0f) * deltaX * 0.01f;
				rotationDelta = FQuat(FVector(0.0f, 1.0f, 0.0f), rotationAmount); // 로컬 Y 축 기준 회전
			}
			else if (Gizmo->GetType() == "CircleZ") {
				float rotationAmount = (cameraForward.x <= 0 ? 1.0f : -1.0f) * deltaX* 0.01f;
				rotationDelta = FQuat(FVector(0.0f, 0.0f, 1.0f),rotationAmount); // 로컬 Z 축 기준 회전
			}

			// 새로운 회전 = 기존 회전 * 변화량
			FQuat newRotation = currentRotation * rotationDelta;


			pObj->SetRotation(newRotation);
		}
		//else if (cdMode == CDM_WORLD)
		//{
		//	FVector cameraForward = GetWorld()->GetCamera()->GetForwardVector();
		//	FVector cameraRight = GetWorld()->GetCamera()->GetRightVector();
		//	FVector cameraUp = GetWorld()->GetCamera()->GetUpVector();

		//	if (Gizmo->GetType() == "CircleX")
		//	{
		//		// X 축 회전 (카메라 방향에 따라 Y 축 이동량 반영)
		//		float rotationAmount = (cameraUp.z >= 0 ? 1.0f : -1.0f) * deltaY;
		//		//FMatrix rotMat = FMatrix::CreateRotation(-rotationAmount, 0.0f, 0.0f);
		//		//FVector newRoc = FMatrix::TransformVector(pObj->GetWorldRotation(), rotMat);
		//		pObj->AddRotation(FVector(-rotationAmount, 0.0f, 0.0f));
		//	}
		//	else if (Gizmo->GetType() == "CircleY")
		//	{
		//		// Y 축 회전 (카메라 방향에 따라 X 축 이동량 반영)
		//		float rotationAmount = (cameraRight.x >= 0 ? -1.0f : 1.0f) * deltaX;
		//		pObj->AddRotation(FVector(0.0f, -rotationAmount, 0.0f));
		//	}
		//	else if (Gizmo->GetType() == "CircleZ")
		//	{
		//		// Z 축 회전 (카메라 방향에 따라 X, Y 이동량 반영)
		//		float rotationAmount = (cameraForward.x <= 0 ? 1.0f : -1.0f) * deltaX;
		//		pObj->AddRotation(FVector(0.0f, 0.0f, rotationAmount));
		//	}
		//}
}

void UPlayer::ControlTranslation(USceneComponent* pObj, UPrimitiveComponent* Gizmo, int32 deltaX, int32 deltaY)
{
	FVector vecObjToCamera = GetWorld()->GetCamera()->GetWorldLocation() - pObj->GetWorldLocation();
	FVector cameraRight = GetWorld()->GetCamera()->GetRightVector();
	FVector cameraUp = GetWorld()->GetCamera()->GetUpVector();
	FVector worldMoveDir = (cameraRight * deltaX + cameraUp * -deltaY) * 0.01f;

	if (cdMode == CDM_LOCAL) {
		if (Gizmo->GetType() == "ArrowX") {
			// 이동 벡터를 로컬 X 축에 투영하여 X 방향 이동 적용
			float moveAmount = worldMoveDir.Dot(pObj->GetForwardVector());
			pObj->AddLocation(pObj->GetForwardVector() * moveAmount);
		}
		else if (Gizmo->GetType() == "ArrowY") {
			// 이동 벡터를 로컬 Y 축에 투영하여 Y 방향 이동 적용
			float moveAmount = worldMoveDir.Dot(pObj->GetRightVector());
			pObj->AddLocation(pObj->GetRightVector() * moveAmount);
		}
		else if (Gizmo->GetType() == "ArrowZ") {
			// 이동 벡터를 로컬 Z 축에 투영하여 Z 방향 이동 적용
			float moveAmount = worldMoveDir.Dot(pObj->GetUpVector());
			pObj->AddLocation(pObj->GetUpVector() * moveAmount);
		}
	}
	else if (cdMode == CDM_WORLD)
	{

		if (Gizmo->GetType() == "ArrowX")
		{
			vecObjToCamera = FVector(vecObjToCamera.x, vecObjToCamera.y, pObj->GetLocalLocation().z);
			float dotResult = vecObjToCamera.Dot(FVector(1.0f, 0.0f, 0.0f));
			dotResult = dotResult / vecObjToCamera.Magnitude();
			float rad = acosf(dotResult);
			float degree = JungleMath::RadToDeg(rad);
			FVector crossResult = vecObjToCamera.Cross(FVector(1.0f, 0.0f, 0.0f));
			if (crossResult.z > 0)
				degree *= -1.0f;
			//UE_LOG(LogLevel::Error, "%f", degree);

			if ( 0 < degree && degree <  180.0f)
				pObj->AddLocation(FVector(1.0f, 0.0f, 0.0f) * deltaX * 0.01f);
			else if (degree < 0 && degree > -180.0f) {
				pObj->AddLocation(FVector(1.0f, 0.0f, 0.0f) * deltaX * -0.01f);
			}
		}
		else if (Gizmo->GetType() == "ArrowY")
		{
			vecObjToCamera = FVector(vecObjToCamera.x, vecObjToCamera.y, pObj->GetLocalLocation().z);
			float dotResult = vecObjToCamera.Dot(FVector(0.0f, 1.0f, 0.0f));
			dotResult = dotResult / vecObjToCamera.Magnitude();
			float rad = acosf(dotResult);
			float degree = JungleMath::RadToDeg(rad);
			FVector crossResult = vecObjToCamera.Cross(FVector(0.0f, 1.0f, 0.0f));
			if (crossResult.z > 0)
				degree *= -1.0f;
			//UE_LOG(LogLevel::Error, "%f", degree);
			if (0 < degree && degree < 180)
				pObj->AddLocation(FVector(0.0f, 1.0f, 0.0f) * deltaX * 0.01f);
			else
				pObj->AddLocation(FVector(0.0f, 1.0f, 0.0f) * deltaX * -0.01f);
		}	
		else if (Gizmo->GetType() == "ArrowZ")
		{
			pObj->AddLocation(FVector(0.0f, 0.0f, 1.0f) * deltaY * -0.01f);
		}
	}
}

void UPlayer::ControlScale(USceneComponent* pObj, UPrimitiveComponent* Gizmo, int32 deltaX, int32 deltaY)
{
	FVector vecObjToCamera = GetWorld()->GetCamera()->GetWorldLocation() - pObj->GetWorldLocation();

	if (Gizmo->GetType() == "ScaleX")
	{
		vecObjToCamera = FVector(vecObjToCamera.x, vecObjToCamera.y, pObj->GetLocalLocation().z);
		float dotResult = vecObjToCamera.Dot(FVector(1.0f, 0.0f, 0.0f));
		dotResult = dotResult / vecObjToCamera.Magnitude();
		float rad = acosf(dotResult);
		float degree = JungleMath::RadToDeg(rad);
		FVector crossResult = vecObjToCamera.Cross(FVector(1.0f, 0.0f, 0.0f));
		if (crossResult.z > 0)
			degree *= -1.0f;
		//UE_LOG(LogLevel::Error, "%f", degree);

		if (0 < degree && degree < 180.0f)
			pObj->AddScale(FVector(1.0f, 0.0f, 0.0f) * deltaX * 0.01f);
		else if (degree < 0 && degree > -180.0f) {
			pObj->AddScale(FVector(1.0f, 0.0f, 0.0f) * deltaX * -0.01f);
		}
	}
	else if (Gizmo->GetType() == "ScaleY")
	{
		vecObjToCamera = FVector(vecObjToCamera.x, vecObjToCamera.y, pObj->GetLocalLocation().z);
		float dotResult = vecObjToCamera.Dot(FVector(0.0f, 1.0f, 0.0f));
		dotResult = dotResult / vecObjToCamera.Magnitude();
		float rad = acosf(dotResult);
		float degree = JungleMath::RadToDeg(rad);
		FVector crossResult = vecObjToCamera.Cross(FVector(0.0f, 1.0f, 0.0f));
		if (crossResult.z > 0)
			degree *= -1.0f;
		//UE_LOG(LogLevel::Error, "%f", degree);
		if (0 < degree && degree < 180)
			pObj->AddScale(FVector(0.0f, 1.0f, 0.0f) * deltaX * 0.01f);
		else
			pObj->AddScale(FVector(0.0f, 1.0f, 0.0f) * deltaX * -0.01f);
	}
	else if (Gizmo->GetType() == "ScaleZ")
	{
		pObj->AddScale(FVector(0.0f, 0.0f, 1.0f) * deltaY * -0.01f);
	}
}
