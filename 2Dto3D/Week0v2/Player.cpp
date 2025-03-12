#include "Player.h"
#include <Windows.h>
#include "GraphicDevice.h"
#include "World.h"
#include "Define.h"
#include "EngineLoop.h"
#include "PrimitiveComponent.h"
#include "JungleMath.h"
#include <DirectXMath.h>
#include "ArrowComp.h"
#include "CameraComponent.h"
#include "LocalGizmoComponent.h"
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
			AddMode();
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
}

void UPlayer::PickGizmo(FVector& pickPosition)
{
	if (GetWorld()->GetPickingObj()) {
		for (auto i : GetWorld()->TestLocalGizmo->GetArrowArr())
		{
			//UArrowComp* Arrow = static_cast<UArrowComp*>(GetWorld()->LocalGizmo[i]);
			float Scale = 0.0f;
			FVector DetectLoc;
			//if (i == 0) {
			//	Scale = GetWorld()->LocalGizmo[0]->GetWorldScale().x;
			//	DetectLoc = Arrow->GetWorldLocation() + GetWorld()->GetPickingObj()->GetRightVector();
			//}
			//else if (i == 1) {
			//	Scale = GetWorld()->LocalGizmo[1]->GetWorldScale().y;
			//	DetectLoc = Arrow->GetWorldLocation() + GetWorld()->GetPickingObj()->GetUpVector();
			//}
			//else if (i == 2) {
			//	Scale = GetWorld()->LocalGizmo[2]->GetWorldScale().z;
			//	DetectLoc = Arrow->GetWorldLocation() + GetWorld()->GetPickingObj()->GetForwardVector();
			//}
			float minDistance = 1000000.0f;
			float Distance = 0.0f;
			if (RayIntersectsObject(rayOrigin, rayDir, GetWorld()->LocalGizmo[i], Distance))
			{
				if (currentIntersectCount > maxIntersect && minDistance > Distance)
				{
					GetWorld()->SetPickingGizmo(i);
					minDistance = Distance;
					maxIntersect = currentIntersectCount;
				}
			}
		}
	}
}

void UPlayer::PickObj(FVector& pickPosition)
{
	UObject* Possible = nullptr;
	int maxIntersect = 0;
	for (auto iter : GetWorld()->GetObjectArr())
	{
		UPrimitiveComponent* pObj = nullptr;
		if (iter->IsA(UPrimitiveComponent::StaticClass())) {
			 pObj = static_cast<UPrimitiveComponent*>(iter);
		}
		if (pObj && pObj->GetType() != "Arrow")
		{
			float minDistance = FLT_MAX;
			float Distance = 0.0f;
			int currentIntersectCount = 0;
			if (RayIntersectsObject(pickPosition, pObj, Distance, currentIntersectCount))
			{
				if (currentIntersectCount > maxIntersect && minDistance > Distance) {
					Possible = pObj;
					minDistance = Distance;
					maxIntersect = currentIntersectCount;
				}
			}
		}
	}
	if (Possible) {
		UE_LOG(LogLevel::Error, dynamic_cast<UPrimitiveComponent*>(Possible)->GetType().c_str());
		GetWorld()->SetPickingObj(Possible);
	}
}

void UPlayer::AddMode()
{
	cMode = static_cast<ControlMode>(((cMode + 1) % ControlMode::CM_END));
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

	std::wstring ws = L"pickPosition:" + std::to_wstring(pickPosition.x) + 
					  L", " + std::to_wstring(pickPosition.y) + 
					  L", " + std::to_wstring(pickPosition.z) + L"\n";
	
	
	OutputDebugString(ws.c_str());
}
int UPlayer::RayIntersectsObject(const FVector& pickPosition, UPrimitiveComponent* obj, float& hitDistance, int& intersectCount)
{
	// 오브젝트의 월드 변환 행렬 생성 (위치, 회전 적용)
	FMatrix rotationMatrix = FMatrix::CreateRotation(
		obj->GetWorldRotation().x,
		obj->GetWorldRotation().y,
		obj->GetWorldRotation().z
	);

	FMatrix translationMatrix = FMatrix::CreateTranslationMatrix(obj->GetWorldLocation());

	// 최종 변환 행렬
	FMatrix worldMatrix = rotationMatrix * translationMatrix;
	FMatrix inverseMatrix = FMatrix::Inverse(worldMatrix * GEngineLoop.View);
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
		UArrowComp* Arrow = static_cast<UArrowComp*>(GetWorld()->GetPickingGizmo());
		switch (cMode)
		{
		case CM_TRANSLATION:
			ControlTranslation(pObj, Arrow, deltaX, deltaY);
			break;
		case CM_SCALE:
			ControlScale(pObj, Arrow, deltaX, deltaY);

			break;
		case CM_ROTATION:
			ControlRoation(Arrow, pObj, deltaX, deltaY);
			break;
		}
		// 새로운 마우스 위치 저장
		m_LastMousePos = currentMousePos;
	}
}

void UPlayer::ControlRoation(UArrowComp* Arrow, USceneComponent* pObj, int32 deltaX, int32 deltaY)
{
	switch (Arrow->GetDir())
	{
	case AD_X:
		if (GetWorld()->GetCamera()->GetForwardVector().z >= 0) {
			pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * deltaX * 0.1f);
			pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * deltaY * 0.1f);
		}
		else {
			pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * deltaX * -0.1f);
			pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * deltaY * -0.1f);
		}

		break;
	case AD_Y:
		if (pObj->GetUpVector().y >= 0)
		{
			pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * deltaX * -0.1f);
			pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * deltaX * -0.1f);
		}
		else {
			pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * deltaY * 0.1f);
			pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * deltaX * 0.1f);
		}
		break;
	case AD_Z:

		if (GetWorld()->GetCamera()->GetForwardVector().x <= 0) {
			pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaX * 0.1f);
			pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaY * 0.1f);
		}
		else {
			pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaX * -0.1f);
			pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaX * -0.1f);
		}
		break;
	default:
		break;
	}
}

void UPlayer::ControlTranslation(USceneComponent* pObj, UArrowComp* Arrow, int32 deltaX, int32 deltaY)
{
	float xdir = pObj->GetRightVector().x >= 0 ? 1.0 : -1.0;
	float zdir = pObj->GetForwardVector().z >= 0 ? 1.0 : -1.0;
	switch (Arrow->GetDir())
	{
	case AD_X:
		if (GetWorld()->GetCamera()->GetForwardVector().z >= 0)
			pObj->AddLocation(pObj->GetRightVector() * deltaX * 0.01f * xdir);
		else
			pObj->AddLocation(pObj->GetRightVector() * deltaX * -0.01f * xdir);
		break;
	case AD_Y:
		if (pObj->GetUpVector().y >= 0)
			pObj->AddLocation((pObj->GetUpVector() * deltaY * 0.01f) * -1);
		else
			pObj->AddLocation((pObj->GetUpVector() * deltaY * 0.01f));
		break;
	case AD_Z:

		if (GetWorld()->GetCamera()->GetForwardVector().x <= 0)
			pObj->AddLocation(pObj->GetForwardVector() * deltaX * 0.01f * zdir);
		else
			pObj->AddLocation(pObj->GetForwardVector() * deltaX * -0.01f * zdir);
		break;
	default:
		break;
	}
}

void UPlayer::ControlScale(USceneComponent* pObj, UArrowComp* Arrow, int32 deltaX, int32 deltaY)
{
	float xdir = pObj->GetRightVector().x >= 0 ? 1.0 : -1.0;
	float zdir = pObj->GetForwardVector().z >= 0 ? 1.0 : -1.0;
	switch (Arrow->GetDir())
	{
	case AD_X:
		if (GetWorld()->GetCamera()->GetForwardVector().z >= 0)
			pObj->AddScale(FVector(1.0f, 0.0f, 0.0f) * deltaX * 0.01f * xdir);
		else
			pObj->AddScale(FVector(1.0f, 0.0f, 0.0f) * deltaX * -0.01f * xdir);
		break;
	case AD_Y:
		if (pObj->GetUpVector().y >= 0)
			pObj->AddScale(FVector(0.0f, 1.0f, 0.0f) * deltaY * -0.01f);
		else
			pObj->AddScale(FVector(0.0f, 1.0f, 0.0f) * deltaY * 0.01f);
		break;
	case AD_Z:

		if (GetWorld()->GetCamera()->GetForwardVector().x <= 0)
			pObj->AddScale(FVector(0.0f, 0.0f, 1.0f) * deltaX * 0.01f * zdir);
		else
			pObj->AddScale(FVector(0.0f, 0.0f, 1.0f) * deltaX * -0.01f * zdir);
		break;
	default:
		break;
	}
}
