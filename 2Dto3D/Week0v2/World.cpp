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

	UObject* localGizmo = FObjectFactory::ConstructObject<UArrowComp>();
	static_cast<UArrowComp*>(localGizmo)->SetScale(FVector(2.0f, 1.0f, 1.0f));
	static_cast<UArrowComp*>(localGizmo)->SetDir(ARROW_DIR::AD_X);
	static_cast<UArrowComp*>(localGizmo)->SetType("ArrowX");
	LocalGizmo[0] = static_cast<UArrowComp*>(localGizmo);
	GUObjectArray.push_back(localGizmo);

	localGizmo = nullptr;
	localGizmo = FObjectFactory::ConstructObject<UArrowComp>();
	static_cast<UArrowComp*>(localGizmo)->SetScale(FVector(1.0f, 2.0f, 1.0f));
	static_cast<UArrowComp*>(localGizmo)->SetDir(ARROW_DIR::AD_Y);
	static_cast<UArrowComp*>(localGizmo)->SetType("ArrowY");
	LocalGizmo[1] = static_cast<UArrowComp*>(localGizmo);
	GUObjectArray.push_back(localGizmo);

	localGizmo = nullptr;
	localGizmo = FObjectFactory::ConstructObject<UArrowComp>();
	static_cast<UArrowComp*>(localGizmo)->SetScale(FVector(1.0f, 1.0f, 2.0f));

	static_cast<UArrowComp*>(localGizmo)->SetDir(ARROW_DIR::AD_Z);
	static_cast<UArrowComp*>(localGizmo)->SetType("ArrowZ");
	LocalGizmo[2] = static_cast<UArrowComp*>(localGizmo);
	GUObjectArray.push_back(localGizmo);
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
		
		FVector temp = FVector(pickingObj->GetScale().x, 0.0f, 0.0f);
		LocalGizmo[0]->SetLocation(pickingObj->GetLocation());

		LocalGizmo[0]->SetRotation(pickingObj->GetRotation());
		LocalGizmo[0]->SetScale(FVector(pickingObj->GetScale().x + 2.0f, 1.0f, 1.0f));
		LocalGizmo[1]->SetLocation(pickingObj->GetLocation());
		LocalGizmo[1]->SetRotation(pickingObj->GetRotation());
		LocalGizmo[1]->SetScale(FVector(1.0f, pickingObj->GetScale().y + 2.0f, 1.0f));

		LocalGizmo[2]->SetLocation(pickingObj->GetLocation());
		LocalGizmo[2]->SetRotation(pickingObj->GetRotation());
		LocalGizmo[2]->SetScale(FVector(1.0f, 1.0f, pickingObj->GetScale().z + 2.0f));

	}
}

void UWorld::Render()
{
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
			if(Primitive->GetType() != "ArrowX" && Primitive->GetType() != "ArrowY" && Primitive->GetType() != "ArrowZ")
			Save.Primitives[Count] = iter;
			Count++;
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
