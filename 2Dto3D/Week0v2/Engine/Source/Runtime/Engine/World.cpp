#include "Engine/Source/Runtime/Engine/World.h"
#include "Engine/Source/Runtime/Engine/Camera/CameraComponent.h"
#include "Components/SphereComp.h"
#include "Components/CubeComp.h"
#include <DirectXMath.h>
#include "Engine/Source/Runtime/Core/Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
#include "Components/Player.h"
#include "BaseGizmos/GizmoArrowComponent.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Components/UBillboardComponent.h"
#include "Components/UText.h"
#include "Components/UParticleSubUVComp.h"
#include "Components/LightComponent.h"
#include "Components/UTextUUID.h"
#include "Components/SkySphereComponent.h"
UWorld::UWorld()
{
}

UWorld::~UWorld()
{
	Release();
}

void UWorld::Initialize()
{
	CreateBaseObject();
}

void UWorld::CreateBaseObject()
{
	UObject* player = FObjectFactory::ConstructObject<UPlayer>("LocalPlayer");
	localPlayer = static_cast<UPlayer*>(player);

	UObject* Camera = FObjectFactory::ConstructObject<UCameraComponent>("MainCamere");
	camera = static_cast<UCameraComponent*>(Camera);
	camera->SetLocation(FVector(8.0f, 8.0f, 8.f));
	camera->SetRotation(FVector(0.0f, 45.0f, -135.0f));


	UObject* tmp = FObjectFactory::ConstructObject<UTransformGizmo>("LocalGizmo");
	LocalGizmo = static_cast<UTransformGizmo*>(tmp);

	////테스트용 텍스트

	UObject* pObj = FObjectFactory::ConstructObject<USkySphereComponent>("SkySphere");
	USkySphereComponent* skySphere = static_cast<USkySphereComponent*>(pObj);
	skySphere->SetTexture(L"Assets/Texture/ocean_sky.jpg");
	skySphere->SetScale(FVector(-300.0f, -300.0f, -300.0f));
	skySphere->SetRotation(FVector(-167.0f, 25.0f, -135.0f));

	GUObjectArray.push_back(skySphere);


	//테스트용 텍스트


/*

	//테스트용 빌보드. 필요없으면 삭제
	UObject* billboard = FObjectFactory::ConstructObject<UBillboardComponent>();
	billboard = static_cast<UBillboardComponent*>(billboard);
	UBillboardComponent* castBillboard = static_cast<UBillboardComponent*>(billboard);
	castBillboard->SetTexture(L"Assets/Texture/emart.png");
	GUObjectArray.push_back(billboard);


	/*
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
		if ((ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_UUIDText)))
			iter->RenderUUID();
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
	case OBJ_SpotLight:
	{
		UObject* spotLight = FObjectFactory::ConstructObject<ULightComponentBase>("SpotLight");
		spotLight = static_cast<ULightComponentBase*>(spotLight);
		ULightComponentBase* castLight = static_cast<ULightComponentBase*>(spotLight);

		GUObjectArray.push_back(spotLight);
		break;
	}
	case OBJ_PARTICLE:
	{
		UObject* particle = FObjectFactory::ConstructObject<UParticleSubUVComp>("FireParticle");
		UParticleSubUVComp* castParticle = static_cast<UParticleSubUVComp*>(particle);
		castParticle->SetTexture(L"Assets/Texture/T_Explosion_SubUV.png");
		castParticle->SetRowColumnCount(6, 6);
		castParticle->SetScale(FVector(10.0f, 10.0f, 1.0f));
		GUObjectArray.push_back(castParticle);
	}
	break;
	case OBJ_Text:
	{
		UObject* text = FObjectFactory::ConstructObject<UText>("Quad");
		UText* castText = static_cast<UText*>(text);
		castText->SetTexture(L"Assets/Texture/font.png");
		castText->SetRowColumnCount(106, 106);
		castText->SetText(L"안녕하세요 Jungle 1");
		//SetText전에 RowColumn 반드시 설정
		GUObjectArray.push_back(text);
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

		USceneComponent* Primitive = nullptr;
		if (iter->IsA(USceneComponent::StaticClass())) {
			Primitive = static_cast<USceneComponent*>(iter);
		}
		if (Primitive && !Primitive->IsA(UBillboardComponent::StaticClass())) {
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
