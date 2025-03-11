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
	ImGui::Begin("Jungle Property Panel");

	UPlayer* player = static_cast<UPlayer*>(world->GetPlayer());
	std::string buttonLabel;
	if(player->GetMode() == ControlMode::CM_TRANSLATION)
	  buttonLabel = "Translation";
	else if (player->GetMode() == ControlMode::CM_ROTATION)
		buttonLabel = "Rotation";
	else if (player->GetMode() == ControlMode::CM_SCALE)
		buttonLabel = "Scale";
	if (ImGui::Button(buttonLabel.c_str())) {
		player->AddMode();
	}
	USceneComponent* PickObj = static_cast<USceneComponent*>(world->GetPickingObj());
	if (PickObj) {
		float pickObjLoc[3] = { PickObj->GetLocation().x,PickObj->GetLocation().y ,PickObj->GetLocation().z };
		float pickObjRot[3] = { PickObj->GetRotation().x,PickObj->GetRotation().y ,PickObj->GetRotation().z };
		float pickObjScale[3] = { PickObj->GetScale().x,PickObj->GetScale().y ,PickObj->GetScale().z };

		ImGui::InputFloat3("Tranlsation", pickObjLoc);
		ImGui::InputFloat3("Rotation", pickObjRot);
		ImGui::InputFloat3("Scale", pickObjScale);

		PickObj->SetLocation(FVector(pickObjLoc[0], pickObjLoc[1], pickObjLoc[2]));
		PickObj->SetRotation(FVector(pickObjRot[0], pickObjRot[1], pickObjRot[2]));
		PickObj->SetScale(FVector(pickObjScale[0], pickObjScale[1], pickObjScale[2]));
	}
	ImGui::End();
}
