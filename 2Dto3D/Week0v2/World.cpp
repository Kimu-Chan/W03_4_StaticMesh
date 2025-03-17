#include "World.h"
#include "CameraComponent.h"
#include "SphereComp.h"
#include "CubeComp.h"
#include <DirectXMath.h>
#include "JungleMath.h"
#include "ObjectFactory.h"
#include "Player.h"
#include "GizmoArrowComponent.h"
#include "TransformGizmo.h"
#include "UBillboardComponent.h"
#include "UText.h"
#include "UParticleSubUVComp.h"
#include "UTextUUID.h"

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
	localPlayer = static_cast<UPlayer*>(player);

	UObject* Camera = FObjectFactory::ConstructObject<UCameraComponent>("MainCamere");
	camera = static_cast<UCameraComponent*>(Camera);
	camera->SetLocation(FVector(0.0f, 10.0f, 0.f));

	UObject* tmp = FObjectFactory::ConstructObject<UTransformGizmo>("LocalGizmo");
	LocalGizmo = static_cast<UTransformGizmo*>(tmp);
	
/*
	//테스트용 빌보드. 필요없으면 삭제
	UObject* billboard = FObjectFactory::ConstructObject<UBillboardComponent>();
	billboard = static_cast<UBillboardComponent*>(billboard);
	UBillboardComponent* castBillboard = static_cast<UBillboardComponent*>(billboard);
	castBillboard->SetTexture(L"Assets/Texture/emart.png");
	GUObjectArray.push_back(billboard);




	UObject* pObj = nullptr;
	pObj = FObjectFactory::ConstructObject<UCubeComp>("Cube");
	UCubeComp* cube = static_cast<UCubeComp*>(pObj);
	GUObjectArray.push_back(pObj);


	UObject* pObj = nullptr;
	pObj = FObjectFactory::ConstructObject<USphereComp>("Sphere");
	USphereComp* sphere = static_cast<USphereComp*>(pObj);
	GUObjectArray.push_back(pObj);
	
	//테스트용 텍스트
	UObject* text = FObjectFactory::ConstructObject<UText>();
	UText* castText = static_cast<UText*>(text);
	castText->SetTexture(L"Assets/Texture/font.png");

	castText->SetRowColumnCount(106, 106);
	castText->SetText(L"안녕하세요 Jungle 1");
	//SetText전에 RowColumn 반드시 설정
	GUObjectArray.push_back(text);

	//테스트용 텍스트
	UObject* uuid = FObjectFactory::ConstructObject<UTextUUID>();
	UTextUUID* castUUID = static_cast<UTextUUID*>(uuid);
	castUUID->SetTexture(L"Assets/Texture/font.png");
	castUUID->SetRowColumnCount(106, 106);
	castUUID->SetUUID(sphere->UUID);
	castUUID->SetScale(FVector(0.25f, 0.25f, 0.25f));
	//SetText전에 RowColumn 반드시 설정
	GUObjectArray.push_back(uuid);

	castUUID->SetUUIDParent(sphere);
	//castBillboard->SetParent(sphere);
	//sphere->AddChild(castBillboard);
	//cube->AddChild(sphere);
*/
}

void UWorld::ReleaseBaseObject()
{
	delete LocalGizmo;
	delete worldGizmo;
	delete camera;
	delete localPlayer;
	LocalGizmo = nullptr;
	camera = nullptr;
	localPlayer = nullptr;
}

void UWorld::RenderBaseObject()
{
	LocalGizmo->Render();
}

void UWorld::Tick(double deltaTime)
{
	Input();
	camera->Update(deltaTime);
	localPlayer->Update(deltaTime);
	LocalGizmo->Update(deltaTime);
	for (auto iter : GUObjectArray)
	{
		iter->Update(deltaTime);
	}
	
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
	ReleaseBaseObject();
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
	case OBJ_PARTICLE:
		{
		UObject* particle = FObjectFactory::ConstructObject<UParticleSubUVComp>();
		UParticleSubUVComp* castParticle = static_cast<UParticleSubUVComp*>(particle);
		castParticle->SetTexture(L"Assets/Texture/T_Explosion_SubUV.PNG");
		castParticle->SetRowColumnCount(6, 6);
		GUObjectArray.push_back(castParticle);
		}
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

void UWorld::DeleteObj(UObject* _Obj)
{
	UObject* tmpObj = _Obj;
	Trashbin.erase(std::find(GUObjectArray.begin(), GUObjectArray.end(), _Obj));
	delete tmpObj;
}

void UWorld::ThrowAwayObj(UObject* _Obj)
{
	Trashbin.push_back(_Obj);
}

void UWorld::CleanUp()
{
	if (Trashbin.empty())
		return;

	for (auto it = Trashbin.begin(); it != Trashbin.end();)
	{
		/*DeleteObj(it);*/
		GUObjectArray.erase(std::remove(GUObjectArray.begin(), GUObjectArray.end(), (*it)), GUObjectArray.end());
		delete (*it);
		it = Trashbin.erase(it);
	}
}

void UWorld::SetPickingGizmo(UObject* _Obj)
{
	pickingGizmo = static_cast<USceneComponent*>(_Obj);
}
