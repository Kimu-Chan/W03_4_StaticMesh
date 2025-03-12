#pragma once
#include "ControlPaner.h"
#include "World.h"
#include "CameraComponent.h"
#include "ImGuiManager.h"
#include "SceneMgr.h"

#include "EditorWindow.h"
// #include "ImGUI\imgui.h"
#include "IWindowToggleable.h"
//#include "Font\IconDefs.h"
//#include "Font/RawFonts.h"
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

	
	ImGuiIO& io = ImGui::GetIO();
	
	//OutputDebugString( (std::to_wstring(io.Fonts->Fonts.Size) + L"\n").c_str());

	ImFont* UnicodeFont = io.Fonts->Fonts[FEATHER_FONT];

	ImVec2 ControlButtonSize = ImVec2(32, 32);
	ImGui::PushFont(UnicodeFont);
	ImVec4 ActiveColor = ImVec4(0, 0.5, 0, 0.6f);

	//bool isTranslationActive = (PrimaryGizmo && PrimaryGizmo->GetCurrentGizmo() == EGizmoType::Translation);
	//if (isTranslationActive)
	//	ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor); // 활성 상태 색상
	if (ImGui::Button(u8"\ue9bc", ControlButtonSize))
	{
		//if (PrimaryGizmo)
		//{
		//	PrimaryGizmo->SetGizmoType(EGizmoType::Translation);
		//}
	}
	if (ImGui::Button(u8"\ue9d3", ControlButtonSize))
	{
		//if (PrimaryGizmo)
		//{
		//	PrimaryGizmo->SetGizmoType(EGizmoType::Rotation);
		//}
	}
	/*if (ImGui::Button(u8"\ue9b7"))
	{
	}*/
	/// Toggle Control Panel
	//if (ImGui::Button(ICON_MONITOR))
	//{
	//	Console::GetInstance().bWasOpen = !Console::GetInstance().bWasOpen;  //  직접 변경
	//}

	ImGui::PopFont();

	ImGui::Separator();

	ImGui::Text("Orthogonal");
	ImGui::SliderFloat("FOV", &Camera->GetFov(), 30.0f, 120.0f);

	float cameraLocation[3] = { Camera->GetLocation().x, Camera->GetLocation().y, Camera->GetLocation().z };
	ImGui::InputFloat3("Camera Location", cameraLocation);

	float cameraRotation[3] = { Camera->GetRotation().x, Camera->GetRotation().y, Camera->GetRotation().z };
	ImGui::InputFloat3("Camera Rotation", cameraRotation);

	Camera->SetLocation(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));
	Camera->SetRotation(FVector(cameraRotation[0], cameraRotation[1], cameraRotation[2]));

	ImGui::End();
}
