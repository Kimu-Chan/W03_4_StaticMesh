#include "World.h"
#include "CameraComponent.h"
#include "SphereComp.h"
#include "CubeComp.h"
#include <DirectXMath.h>
#include "JungleMath.h"
#include "GizmoComponent.h"
#include "ObjectFactory.h"
#include "Player.h"
#include "ArrowComp.h"
#include "LocalGizmoComponent.h"
UWorld::UWorld()
{
}

UWorld::~UWorld()
{
	Release();
}

void UWorld::Initialize()
{
	CreateBaseObject();;
}

void UWorld::CreateBaseObject()
{
	UObject* player = FObjectFactory::ConstructObject<UPlayer>();
	GUObjectArray.push_back(player);
	localPlayer = static_cast<UPlayer*>(player);

	UObject* Camera = FObjectFactory::ConstructObject<UCameraComponent>();
	camera = static_cast<UCameraComponent*>(Camera);
	camera->SetLocation(FVector(0, 0, -10.f));
	GUObjectArray.push_back(camera);

	UObject* gizmo = FObjectFactory::ConstructObject<UGizmoComponent>();
	static_cast<UGizmoComponent*>(gizmo)->SetScale(FVector(100000.0f, 100000.0f, 100000.0f));
	GUObjectArray.push_back(gizmo);


	UObject* tmp = FObjectFactory::ConstructObject<ULocalGizmoComponent>();
	LocalGizmo = static_cast<ULocalGizmoComponent*>(tmp);
	GUObjectArray.push_back(tmp);
}

void UWorld::Tick(double deltaTime)
{
	Input();

	for (auto iter : GUObjectArray)
	{
		iter->Update(deltaTime);
	}
	UpdateLocalGizmo();
}

void UWorld::Release()
{

	for (auto iter : GUObjectArray)
	{
		delete iter;
	}
	GUObjectArray.clear();
	LocalGizmo = nullptr;
	pickingObj = nullptr;
	pickingGizmo = nullptr;
	worldGizmo = nullptr;
	camera = nullptr;
	localPlayer = nullptr;
}
void UWorld::UpdateLocalGizmo()
{
	if (pickingObj)
	{

		//LocalGizmo->SetLocation(pickingObj->GetWorldLocation());
		//LocalGizmo->SetRotation(pickingObj->GetWorldRotation());
		//LocalGizmo->SetScale(pickingObj->GetWorldScale());
	}
}

void UWorld::Render()
{
	for (auto iter : GUObjectArray)
	{
		iter->Render();
	}
}

void UWorld::Input()
{

}

void UWorld::SpawnObject(OBJECTS _Obj)
{
	UObject* pObj = nullptr;
	switch (_Obj)
	{
	case OBJ_SPHERE:
		pObj = FObjectFactory::ConstructObject<USphereComp>();
		GUObjectArray.push_back(pObj);
		break;
	case OBJ_TRIANGLE:
		break;
	case OBJ_CUBE:
		pObj = FObjectFactory::ConstructObject<UCubeComp>();
		GUObjectArray.push_back(pObj);

		break;
	default:
		break;
	}
	pickingObj = static_cast<USceneComponent*>(pObj);
}

void UWorld::LoadData(SceneData& _Data)
{
	Release();
	Initialize();
	for (auto iter : _Data.Primitives)
	{
		GUObjectArray.push_back(iter.second);
	}
}

SceneData UWorld::SaveData()
{
	SceneData Save;
	int32 Count = 0;
	for (auto iter : GUObjectArray)
	{

		UPrimitiveComponent* Primitive = nullptr;
		if (iter->IsA(UPrimitiveComponent::StaticClass())) {
			Primitive = static_cast<UPrimitiveComponent*>(iter);
		}
		if (Primitive)
		{
			if (Primitive->GetType() != "ArrowX" && Primitive->GetType() != "ArrowY" && Primitive->GetType() != "ArrowZ") {
				Save.Primitives[Count] = iter;
				Count++;
			}
		}
	}
	Save.Version = 1;
	Save.NextUUID = Count;
	
	return Save;
}

void UWorld::NewScene()
{
	Release();
	Initialize();
}

void UWorld::SetPickingObj(UObject* _Obj)
{
	 pickingObj = static_cast<USceneComponent*>(_Obj); 
}

void UWorld::SetPickingGizmo(UObject* _Obj)
{
	pickingGizmo = static_cast<USceneComponent*>(_Obj);
}
