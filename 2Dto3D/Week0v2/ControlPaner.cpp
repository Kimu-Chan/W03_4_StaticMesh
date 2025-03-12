#include "ControlPaner.h"
#include "World.h"
#include "CameraComponent.h"
#include "ImGuiManager.h"
#include "SceneMgr.h"
ControlPanel::ControlPanel()
{
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::Draw(UWorld* world, double elapsedTime )
{
	UCameraComponent* Camera = static_cast<UCameraComponent*>(world->GetCamera());

	ImGui::Begin("Jungle Control Panel");
	ImGui::Text("Hello Jungle World!");
	double fps = 1000.0 / elapsedTime;
	ImGui::Text("FPS %.2f (%.2fms)", fps, elapsedTime);

	ImGui::Separator();
	static int32 primitiveType = 0;
	const char* primitives[] = { "Sphere", "Cube", "Triangle" };
	ImGui::Combo("Primitive", &primitiveType, primitives, IM_ARRAYSIZE(primitives));

	int32 SpawnCount = world->GetObjectArr().size();
	ImGui::InputInt("Number of Spawn", &SpawnCount, 0, 0);
	if (ImGui::Button("Spawn"))
	{
		world->SpawnObject(static_cast<OBJECTS>(primitiveType));
	}

	static char sceneName[64] = "Default";
	ImGui::InputText("Scene Name", sceneName, IM_ARRAYSIZE(sceneName));

	if (ImGui::Button("New scene")) {
		world->NewScene();
	}
	if (ImGui::Button("Save scene")) {
		FString SceneName(sceneName);
		SceneData SaveData = world->SaveData();
		FSceneMgr::SaveSceneToFile(SceneName, SaveData);
	}
	if (ImGui::Button("Load scene")) {
		FString SceneName(sceneName);
		FString LoadJsonData = FSceneMgr::LoadSceneFromFile(SceneName);
		SceneData LoadData = FSceneMgr::ParseSceneData(LoadJsonData);
		world->LoadData(LoadData);
	}

	ImGui::Separator();

	ImGui::Text("Orthogonal");
	ImGui::SliderFloat("FOV", &Camera->GetFov(), 30.0f, 120.0f);

	float cameraLocation[3] = { Camera->GetWorldLocation().x, Camera->GetWorldLocation().y, Camera->GetWorldLocation().z };
	ImGui::InputFloat3("Camera Location", cameraLocation);

	float cameraRotation[3] = { Camera->GetWorldRotation().x, Camera->GetWorldRotation().y, Camera->GetWorldRotation().z };
	ImGui::InputFloat3("Camera Rotation", cameraRotation);

	Camera->SetLocation(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));
	Camera->SetRotation(FVector(cameraRotation[0], cameraRotation[1], cameraRotation[2]));

	ImGui::End();
}
