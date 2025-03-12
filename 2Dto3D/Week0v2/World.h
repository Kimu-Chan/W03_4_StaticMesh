#pragma once
#include "Define.h"
#include "SceneMgr.h"
class UObject;
class UArrowComp;
class UCameraComponent;
class UPlayer;
class USceneComponent;
class ULocalGizmoComponent;
class UWorld
{
public:
	UWorld();
	~UWorld();


	void Initialize();
	void CreateBaseObject();
	void Tick(double deltaTime);
	void UpdateLocalGizmo();
	void Release();
	void Render();
	void Input();
	void SpawnObject(OBJECTS _Obj);
	void LoadData(SceneData& _Data);
	SceneData SaveData();
	void NewScene();
	void	SetPickingObj(UObject* _Obj); 
private:

	TArray<UObject*> GUObjectArray;
	USceneComponent* pickingObj = nullptr;
	UObject* pickingGizmo = nullptr;
	UObject* worldGizmo = nullptr;
	UCameraComponent* camera = nullptr;
	UPlayer* localPlayer = nullptr;
public:
	TArray<UObject*>& GetObjectArr() { return GUObjectArray; }

	ULocalGizmoComponent* LocalGizmo = nullptr;
	UCameraComponent* GetCamera() { return camera; }
	UPlayer* GetPlayer() { return localPlayer; }


	USceneComponent* GetPickingObj() { return pickingObj; }
	UObject* GetWorldGizmo() { return worldGizmo; }
	UObject* GetPickingGizmo() { return pickingGizmo; }
	void	SetPickingGizmo(UObject* _Obj) { pickingGizmo = _Obj; };

};

