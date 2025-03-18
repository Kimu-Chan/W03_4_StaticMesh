#include "PropertyPanel.h"
#include "World.h"
#include "ImGuiManager.h"
#include "Object.h"
#include "Player.h"
#include "SceneComponent.h"
PropertyPanel::PropertyPanel()
{
}

PropertyPanel::~PropertyPanel()
{
}

void PropertyPanel::Draw(UWorld* world)
{
	float controllWindowWidth = static_cast<float>(width) * 0.178f;
	float controllWindowHeight = static_cast<float>(height) * 0.15f;

	float controllWindowPosX = (static_cast<float>(width) - controllWindowWidth) * 0.f;
	float controllWindowPosY = (static_cast<float>(height) - controllWindowHeight) * .75f;

	// 창 크기와 위치 설정
	ImGui::SetNextWindowPos(ImVec2(controllWindowPosX, controllWindowPosY));
	ImGui::SetNextWindowSize(ImVec2(controllWindowWidth, controllWindowHeight), ImGuiCond_Always);

	ImGui::Begin("Jungle Property Panel");

	UPlayer* player = static_cast<UPlayer*>(world->GetPlayer());	
	std::string buttonLabel;

	if(player->GetControlMode() == ControlMode::CM_TRANSLATION)
	  buttonLabel = "Translation";
	else if (player->GetControlMode() == ControlMode::CM_ROTATION)
		buttonLabel = "Rotation";
	else if (player->GetControlMode() == ControlMode::CM_SCALE)
		buttonLabel = "Scale";

	std::string coordiButtonLabel;

	if (player->GetCoordiMode() == CoordiMode::CDM_WORLD)
		coordiButtonLabel = "World";
	else if (player->GetCoordiMode() == CoordiMode::CDM_LOCAL)
		coordiButtonLabel = "Local";
	// 버튼 사이즈 동일하게

	ImVec2 buttonSize(120, 30);

	ImGui::SetNextItemWidth(buttonSize.x); // 버튼 너비 고정
	if (ImGui::Button(buttonLabel.c_str(), buttonSize)) {
		player->AddControlMode(); // 기존 코드 유지
	}
	ImGui::SameLine();
	if (ImGui::Button(coordiButtonLabel.c_str(), buttonSize)) {
		player->AddCoordiMode();
	}
	USceneComponent* PickObj = static_cast<USceneComponent*>(world->GetPickingObj());
	if (PickObj) {
		float pickObjLoc[3] = { PickObj->GetWorldLocation().x,PickObj->GetWorldLocation().y ,PickObj->GetWorldLocation().z };
		float pickObjRot[3] = { PickObj->GetWorldRotation().x,PickObj->GetWorldRotation().y ,PickObj->GetWorldRotation().z };
		float pickObjScale[3] = { PickObj->GetWorldScale().x,PickObj->GetWorldScale().y ,PickObj->GetWorldScale().z };

		ImGui::InputFloat3("Tranlsation", pickObjLoc);
		ImGui::InputFloat3("Rotation", pickObjRot);
		ImGui::InputFloat3("Scale", pickObjScale);

		PickObj->SetLocation(FVector(pickObjLoc[0], pickObjLoc[1], pickObjLoc[2]));
		PickObj->SetRotation(FVector(pickObjRot[0], pickObjRot[1], pickObjRot[2]));
		// 함정카드 발동! z축이 반대로 돌아가지만 여기서 보정해주는것
		PickObj->SetScale(FVector(pickObjScale[0], pickObjScale[1], pickObjScale[2]));
	}

	ImGui::End();
}
void PropertyPanel::OnResize(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;
	
}
