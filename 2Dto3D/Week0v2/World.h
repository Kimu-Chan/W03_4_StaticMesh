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
	void ReleaseBaseObject();
	void RenderBaseObject();
	void Tick(double deltaTime);
	void Release();
	void Render();
	void Input();
	void SpawnObject(OBJECTS _Obj);
	void LoadData(SceneData& _Data);
	SceneData SaveData();
	void	NewScene();
	void	SetPickingObj(UObject* _Obj); 
	void	DeleteObj(UObject* _Obj);
	void	ThrowAwayObj(UObject* _Obj);
	void	CleanUp();
	
private:

	TArray<UObject*> GUObjectArray;
	TArray<UObject*> Trashbin;

	USceneComponent* pickingObj = nullptr;
	USceneComponent* pickingGizmo = nullptr;
	UCameraComponent* camera = nullptr;
	UPlayer* localPlayer = nullptr;
public:
	UObject* worldGizmo = nullptr;
	TArray<UObject*>& GetObjectArr() { return GUObjectArray; }

	ULocalGizmoComponent* LocalGizmo = nullptr;
	UCameraComponent* GetCamera() { return camera; }
	UPlayer* GetPlayer() { return localPlayer; }


	USceneComponent* GetPickingObj() { return pickingObj; }
	UObject* GetWorldGizmo() { return worldGizmo; }
	USceneComponent* GetPickingGizmo() { return pickingGizmo; }
	void	SetPickingGizmo(UObject* _Obj);
};

