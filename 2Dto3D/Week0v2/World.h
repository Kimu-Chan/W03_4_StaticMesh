#pragma once
#include "Define.h"
#include "SceneMgr.h"
class UObject;

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
private:
	//TArray<TDoubleLinkedList<UObject*>> m_pObjectList;

	TArray<UObject*> GUObjectArray;
	UObject* pickingObj = nullptr;
	UObject* pickingGizmo = nullptr;
	UObject* worldGizmo = nullptr;
	UObject* camera = nullptr;
	UObject* localPlayer = nullptr;
public:
	TArray<UObject*> GetObjectArr() { return GUObjectArray; }

	UObject* LocalGizmo[3] = { nullptr, nullptr, nullptr };
	UObject* GetCamera() { return camera; }
	UObject* GetPlayer() { return localPlayer; }

	UObject* GetPickingObj() { return pickingObj; }
	void	SetPickingObj(UObject* _Obj) { pickingObj = _Obj; };
	UObject* GetWorldGizmo() { return worldGizmo; }
	UObject* GetPickingGizmo() { return pickingGizmo; }
	void	SetPickingGizmo(UObject* _Obj) { pickingGizmo = _Obj; };

};

