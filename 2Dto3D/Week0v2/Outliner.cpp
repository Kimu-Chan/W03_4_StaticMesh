#include "Outliner.h"
#include "World.h"
#include "PrimitiveComponent.h"

Outliner::Outliner()
{
}

Outliner::~Outliner()
{
}

Outliner& Outliner::GetInstance()
{
	static Outliner instance;
	return instance;
}

void Outliner::Draw(UWorld* world)
{
    if (ImGui::Begin("OutLiner")) // 윈도우 시작
    {
        if (ImGui::TreeNode("Primitives")) // 트리 노드 생성
        {
            static int selected = -1; // 선택된 항목 저장용 변수

            // 오브젝트 리스트
            for (int32 i = 0; i < world->GetObjectArr().size();i++)
            {
                if(!world->GetObjectArr()[i]->IsA(UPrimitiveComponent::StaticClass()))
                    continue;
                // 선택 가능 항목 (Selectable)
                if (ImGui::Selectable(world->GetObjectArr()[i]->GetName().ToString().c_str(), selected == i))
                {
                    selected = i; // 선택된 아이템 업데이트
                    world->SetPickingObj(world->GetObjectArr()[i]);
                }
            }
            ImGui::TreePop(); // 트리 닫기
        }
    }
    ImGui::End(); // 윈도우 종료
}

void Outliner::OnResize(HWND hWnd)
{
}
