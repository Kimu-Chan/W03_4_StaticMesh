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
#include "DiscHellowComponent.h"
#include "ScaleGizmoComponent.h"
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
		if (pObj && pObj->GetType() != "ArrowX" && pObj->GetType() != "ArrowY" && pObj->GetType() != "ArrowZ"
			&& pObj->GetType() != "DiscX"&& pObj->GetType() != "DiscY"&& pObj->GetType() != "DiscZ"
			&& pObj->GetType() != "ScaleX" && pObj->GetType() != "ScaleY" && pObj->GetType() != "ScaleZ")
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
	//if (cdMode == CoordiMode::CDM_WORLD)
	//	cdMode = CoordiMode::CDM_LOCAL;
	//else if (cdMode == CoordiMode::CDM_LOCAL)
	//	cdMode = CoordiMode::CDM_WORLD;
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

	FMatrix rotationMatrix = FMatrix::CreateRotation(
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
			ControlRoation(pObj, Gizmo, deltaX, deltaY);
			break;
		}
		// 새로운 마우스 위치 저장
		m_LastMousePos = currentMousePos;
	}
}

void UPlayer::ControlRoation(USceneComponent* pObj, UPrimitiveComponent* Gizmo, int32 deltaX, int32 deltaY)
{
		if (cdMode == CDM_LOCAL) {
		//if (Gizmo->GetType() == "ArrowX")
		//{
		//	if (GetWorld()->GetCamera()->GetForwardVector().z >= 0)
		//		pObj->AddLocation(pObj->GetRightVector() * deltaX * 0.01f * xdir);
		//	else
		//		pObj->AddLocation(pObj->GetRightVector() * deltaX * -0.01f * xdir);
		//}
		//else if (Gizmo->GetType() == "ArrowY")
		//{
		//	if (pObj->GetUpVector().y >= 0)
		//		pObj->AddLocation((pObj->GetUpVector() * deltaY * 0.01f) * -1);
		//	else
		//		pObj->AddLocation((pObj->GetUpVector() * deltaY * 0.01f));
		//}
		//else if (Gizmo->GetType() == "ArrowZ")
		//{
		//	if (GetWorld()->GetCamera()->GetForwardVector().x <= 0)
		//		pObj->AddLocation(pObj->GetForwardVector() * deltaX * 0.01f * zdir);
		//	else
		//		pObj->AddLocation(pObj->GetForwardVector() * deltaX * -0.01f * zdir);
		//}

	}
	else if (cdMode == CDM_WORLD)
	{
		if (Gizmo->GetType() == "DiscX")
		{
				if (GetWorld()->GetCamera()->GetForwardVector().z >= 0) {
					pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * deltaX);
					pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * deltaY);
				}
				else {
					pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * -deltaX);
					pObj->AddRotation(FVector(1.0f, 0.0f, 0.0f) * -deltaY);
				}
		}
		else if (Gizmo->GetType() == "DiscY")
		{
			if (pObj->GetUpVector().y >= 0)
			{
				pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * -deltaX);
				pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * -deltaX);
			}
			else {
				pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * deltaY);
				pObj->AddRotation(FVector(0.0f, 1.0f, 0.0f) * deltaX);
			}
		}
		else if (Gizmo->GetType() == "DiscZ")
		{

			if (GetWorld()->GetCamera()->GetForwardVector().x <= 0) {
				pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaX);
				pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaY);
			}
			else {
				pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaX);
				pObj->AddRotation(FVector(0.0f, 0.0f, 1.0f) * deltaX);
			};
		}
	}
}

void UPlayer::ControlTranslation(USceneComponent* pObj, UPrimitiveComponent* Gizmo, int32 deltaX, int32 deltaY)
{
	float xdir = pObj->GetRightVector().x >= 0 ? 1.0 : -1.0;
	float zdir = pObj->GetForwardVector().z >= 0 ? 1.0 : -1.0;
	
	if (cdMode == CDM_LOCAL) {
		if (Gizmo->GetType() == "ArrowX")
		{
			if (GetWorld()->GetCamera()->GetForwardVector().z >= 0)
				pObj->AddLocation(pObj->GetRightVector() * deltaX * 0.01f * xdir);
			else
				pObj->AddLocation(pObj->GetRightVector() * deltaX * -0.01f * xdir);
		}
		else if (Gizmo->GetType() == "ArrowY")
		{
			if (pObj->GetUpVector().y >= 0)
				pObj->AddLocation((pObj->GetUpVector() * deltaY * 0.01f) * -1);
			else
				pObj->AddLocation((pObj->GetUpVector() * deltaY * 0.01f));
		}
		else if (Gizmo->GetType() == "ArrowZ")
		{
			if (GetWorld()->GetCamera()->GetForwardVector().x <= 0)
				pObj->AddLocation(pObj->GetForwardVector() * deltaX * 0.01f * zdir);
			else
				pObj->AddLocation(pObj->GetForwardVector() * deltaX * -0.01f * zdir);
		}

	}
	else if (cdMode == CDM_WORLD)
	{
		if (Gizmo->GetType() == "ArrowX")
		{
			if (GetWorld()->GetCamera()->GetForwardVector().z >= 0)
				pObj->AddLocation(FVector(1.0f, 0.0f, 0.0f) * deltaX * 0.01f);
			else
				pObj->AddLocation(FVector(1.0f, 0.0f, 0.0f) * deltaX * -0.01f);
		}
		else if (Gizmo->GetType() == "ArrowY")
		{
			pObj->AddLocation(FVector(0.0f, 1.0f, 0.0f) * deltaY * -0.01f);
		}
		else if (Gizmo->GetType() == "ArrowZ")
		{

			if (GetWorld()->GetCamera()->GetForwardVector().x <= 0)
				pObj->AddLocation(FVector(0.0f, 0.0f, 1.0f) * deltaX * 0.01f);
			else
				pObj->AddLocation(FVector(0.0f, 0.0f, 1.0f) * deltaX * -0.01f);
		}
	}
}

void UPlayer::ControlScale(USceneComponent* pObj, UPrimitiveComponent* Gizmo, int32 deltaX, int32 deltaY)
{
	float xdir = pObj->GetRightVector().x >= 0 ? 1.0 : -1.0;
	float zdir = pObj->GetForwardVector().z >= 0 ? 1.0 : -1.0;
	if (Gizmo->GetType() == "ScaleX")
	{
		if (GetWorld()->GetCamera()->GetForwardVector().z >= 0)
			pObj->AddScale(FVector(1.0f, 0.0f, 0.0f) * deltaX * 0.01f * xdir);
		else
			pObj->AddScale(FVector(1.0f, 0.0f, 0.0f) * deltaX * -0.01f * xdir);
	}
	else if (Gizmo->GetType() == "ScaleY")
	{
		if (pObj->GetUpVector().y >= 0)
			pObj->AddScale(FVector(0.0f, 1.0f, 0.0f) * deltaY * -0.01f);
		else
			pObj->AddScale(FVector(0.0f, 1.0f, 0.0f) * deltaY * 0.01f);
	}
	else if (Gizmo->GetType() == "ScaleZ")
	{
		if (GetWorld()->GetCamera()->GetForwardVector().x <= 0)
			pObj->AddScale(FVector(0.0f, 0.0f, 1.0f) * deltaX * 0.01f * zdir);
		else
			pObj->AddScale(FVector(0.0f, 0.0f, 1.0f) * deltaX * -0.01f * zdir);
	}
}
