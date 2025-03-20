#include "PropertyEditor/PropertyPanel.h"
#include "World.h"
#include "ImGuiManager.h"
#include "UObject/Object.h"
#include "Components/Player.h"
#include "Components/PrimitiveComponent.h"
#include "Components/LightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/UText.h"
PropertyPanel::PropertyPanel()
{
}

PropertyPanel::~PropertyPanel()
{
}

void PropertyPanel::Draw(UWorld* world)
{
	float controllWindowWidth = static_cast<float>(width) * 0.178f;
	float controllWindowHeight = static_cast<float>(height) * 0.43f;

	float controllWindowPosX = (static_cast<float>(width) - controllWindowWidth) * 0.f;
	float controllWindowPosY = (static_cast<float>(height) - controllWindowHeight) *1.f;

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
    if (PickObj)
    {
        FString objectName = PickObj->GetName().ToString();
        ImGui::Text("%s", objectName.c_str());

        // 위치/회전/스케일을 float[3]에 담아둠
        float pickObjLoc[3] = {
            PickObj->GetWorldLocation().x,
            PickObj->GetWorldLocation().y,
            PickObj->GetWorldLocation().z
        };
        float pickObjRot[3] = {
            PickObj->GetWorldRotation().x,
            PickObj->GetWorldRotation().y,
            PickObj->GetWorldRotation().z
        };
        float pickObjScale[3] = {
            PickObj->GetWorldScale().x,
            PickObj->GetWorldScale().y,
            PickObj->GetWorldScale().z
        };

        // ---------- 위치 (X/Y/Z) ----------
        ImGui::Text("Position");
        ImGui::PushItemWidth(50.0f);
        ImGui::DragFloat("##posX", &pickObjLoc[0], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::SameLine();
        ImGui::DragFloat("##posY", &pickObjLoc[1], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::SameLine();
        ImGui::DragFloat("##posZ", &pickObjLoc[2], 0.6f, -FLT_MAX, FLT_MAX);
        // ---------- 회전 (X/Y/Z) ----------
        ImGui::Text("Rotation");
        ImGui::DragFloat("##rotX", &pickObjRot[0], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::SameLine();
        ImGui::DragFloat("##rotY", &pickObjRot[1], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::SameLine();
        ImGui::DragFloat("##rotZ", &pickObjRot[2], 0.6f, -FLT_MAX, FLT_MAX);

        // ---------- 스케일 (X/Y/Z) ----------
        ImGui::Text("Scale");
        ImGui::DragFloat("##scaleX", &pickObjScale[0], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::SameLine();
        ImGui::DragFloat("##scaleY", &pickObjScale[1], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::SameLine();
        ImGui::DragFloat("##scaleZ", &pickObjScale[2], 0.6f, -FLT_MAX, FLT_MAX);
        ImGui::PopItemWidth();
        // 변경된 값 적용
        PickObj->SetLocation(FVector(pickObjLoc[0], pickObjLoc[1], pickObjLoc[2]));
        PickObj->SetRotation(FVector(pickObjRot[0], pickObjRot[1], pickObjRot[2]));
        PickObj->SetScale(FVector(pickObjScale[0], pickObjScale[1], pickObjScale[2]));
        bool reclaimFocus = false;

        // -------- SpotLight 처리부 ----------
        if (objectName.rfind("SpotLight_", 0) == 0)
        {
            ULightComponentBase* lightObj = dynamic_cast<ULightComponentBase*>(PickObj);
            FVector4 currColor = lightObj->GetColor();

            float r = currColor.x;
            float g = currColor.y;
            float b = currColor.z;
            float a = currColor.a;
            float h, s, v;
            float lightColor[4] = { r, g, b, a };

            // 컬러 픽커 (ColorPicker4)
            if (ImGui::ColorPicker4("SpotLight Color", lightColor,
                ImGuiColorEditFlags_DisplayRGB |
                ImGuiColorEditFlags_NoSidePreview |
                ImGuiColorEditFlags_NoInputs |
                ImGuiColorEditFlags_Float))

            {

                r = lightColor[0];
                g = lightColor[1];
                b = lightColor[2];
                a = lightColor[3];
                lightObj->SetColor(FVector4(r, g, b, a));
            }
            RGBToHSV(r, g, b, h, s, v);
            // RGB/HSV 슬라이더 동기화
            bool changedRGB = false;
            bool changedHSV = false;

            // RGB 슬라이더
            ImGui::PushItemWidth(50.0f);
            if (ImGui::DragFloat("R##R", &r, 0.001f, 0.f, 1.f)) changedRGB = true;
            ImGui::SameLine();
            if (ImGui::DragFloat("G##G", &g, 0.001f, 0.f, 1.f)) changedRGB = true;
            ImGui::SameLine();
            if (ImGui::DragFloat("B##B", &b, 0.001f, 0.f, 1.f)) changedRGB = true;

            // HSV 슬라이더
            if (ImGui::DragFloat("H##H", &h, 0.1f, 0.f, 360)) changedHSV = true;
            ImGui::SameLine();
            if (ImGui::DragFloat("S##S", &s, 0.001f, 0.f, 1)) changedHSV = true;
            ImGui::SameLine();
            if (ImGui::DragFloat("V##V", &v, 0.001f, 0.f, 1)) changedHSV = true;
            ImGui::PopItemWidth();

            if (changedRGB && !changedHSV)
            {
                // RGB만 변경 -> HSV 동기화
                RGBToHSV(r, g, b, h, s, v);
                lightObj->SetColor(FVector4(r, g, b, a));
            }
            else if (changedHSV && !changedRGB)
            {
                // HSV만 변경 -> RGB 동기화
                HSVToRGB(h, s, v, r, g, b);
                lightObj->SetColor(FVector4(r, g, b, a));
            }

            // 라이트 반경(Radius)
            float radiusVal = lightObj->GetRadius();
            if (ImGui::SliderFloat("Radius", &radiusVal, 1.0f, 100.0f))
            {
                lightObj->SetRadius(radiusVal);
            }
        }
        if (objectName.rfind("Quad", 0) == 0) {
            UText* textOBj = dynamic_cast<UText*>(PickObj);
            if (textOBj) {
                textOBj->SetTexture(L"Assets/Texture/font.png");
                textOBj->SetRowColumnCount(106, 106);
                FWString wText = textOBj->GetText();
                int len = WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, nullptr, 0, nullptr, nullptr);
                FString u8Text(len, '\0');
                WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, &u8Text[0], len, nullptr, nullptr);

                static char buf[256];
                strcpy_s(buf, u8Text.c_str());
               
                if (ImGui::InputText(u8"수정", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    textOBj->ClearText();
                    int wlen = MultiByteToWideChar(CP_UTF8, 0, buf, -1, nullptr, 0);
                    FWString newWText(wlen, L'\0');
                    MultiByteToWideChar(CP_UTF8, 0, buf, -1, &newWText[0], wlen);
                    textOBj->SetText(newWText.c_str());
                   
                    reclaimFocus = true;
                }
                if (reclaimFocus) {
                    ImGui::SetKeyboardFocusHere(1);
                }
            }
        }
    }

 	ImGui::End();
}
void PropertyPanel::RGBToHSV(float r, float g, float b, float& h, float& s, float& v)
{
	float mx = max(r, max(g, b));
	float mn = min(r, min(g, b));
	float delta = mx - mn;

	v = mx;

	if (mx == 0.0f) {
		s = 0.0f;
		h = 0.0f;
		return;
	}
	else {
		s = delta / mx;
	}

	if (delta < 1e-6) {
		h = 0.0f;
	}
	else {
		if (r >= mx) {
			h = (g - b) / delta;
		}
		else if (g >= mx) {
			h = 2.0f + (b - r) / delta;
		}
		else {
			h = 4.0f + (r - g) / delta;
		}
		h *= 60.0f;
		if (h < 0.0f) {
			h += 360.0f;
		}
	}
}
void PropertyPanel::OnResize(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;
	
}
