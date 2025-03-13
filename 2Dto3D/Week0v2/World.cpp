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
#include "UBillboardComponent.h"
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
	UObject* player = FObjectFactory::ConstructObject<UPlayer>("LocalPlayer");
	GUObjectArray.push_back(player);
	localPlayer = static_cast<UPlayer*>(player);

	UObject* Camera = FObjectFactory::ConstructObject<UCameraComponent>("MainCamere");
	camera = static_cast<UCameraComponent*>(Camera);
	camera->SetLocation(FVector(0.0f, 10.0f, 0.f));
	GUObjectArray.push_back(camera);

	UObject* gizmo = FObjectFactory::ConstructObject<UGizmoComponent>("WorldGizmo");
	static_cast<UGizmoComponent*>(gizmo)->SetScale(FVector(100000.0f, 100000.0f, 100000.0f));
	GUObjectArray.push_back(gizmo);


	UObject* tmp = FObjectFactory::ConstructObject<ULocalGizmoComponent>("LocalGizmo");
	LocalGizmo = static_cast<ULocalGizmoComponent*>(tmp);
	GUObjectArray.push_back(tmp);

	//테스트용 빌보드. 필요없으면 삭제
	UObject* billboard = FObjectFactory::ConstructObject<UBillboardComponent>();
	billboard = static_cast<UBillboardComponent*>(billboard);
	GUObjectArray.push_back(billboard);
}

void UWorld::Tick(double deltaTime)
{
	Input();

	for (auto iter : GUObjectArray)
	{
		iter->Update(deltaTime);
	}
	//if(!Trashbin.empty())
	//	CleanUp();
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
		pObj = FObjectFactory::ConstructObject<USphereComp>("Sphere");
		GUObjectArray.push_back(pObj);
		break;
	case OBJ_TRIANGLE:
		break;
	case OBJ_CUBE:
		pObj = FObjectFactory::ConstructObject<UCubeComp>("Cube");
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

void UWorld::DeleteObj(UObject* _Obj)
{
	UObject* tmpObj = _Obj;
	GUObjectArray.erase(std::find(GUObjectArray.begin(),GUObjectArray.end(), _Obj));
	delete tmpObj;
}

void UWorld::ThrowAwayObj(UObject* _Obj)
{
	Trashbin.push_back(_Obj);
}

void UWorld::CleanUp()
{
	for (auto it : Trashbin)
	{
		DeleteObj(it);
	}
	Trashbin.clear();
}

void UWorld::SetPickingGizmo(UObject* _Obj)
{
	pickingGizmo = static_cast<USceneComponent*>(_Obj);
}
