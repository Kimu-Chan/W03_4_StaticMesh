#include "GizmoComponent.h"
#include "JungleMath.h"
#include "World.h"
UGizmoComponent::UGizmoComponent()
{
}

UGizmoComponent::~UGizmoComponent()
{
}

void UGizmoComponent::Initialize()
{
	Super::Initialize();
}

void UGizmoComponent::Update(double deltaTime)
{
}

void UGizmoComponent::Release()
{
}

void UGizmoComponent::Render()
{
	FEngineLoop::graphicDevice.DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());

	// 최종 MVP 행렬
	FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	if (this == GetWorld()->GetPickingObj()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);
	FStaticMesh* mesh = FEngineLoop::resourceMgr.GetMesh("Gizmo").get();
	FEngineLoop::renderer.RenderPrimitive(mesh->vertexBuffer, mesh->numVertices);
	//Super::Render();
	FEngineLoop::graphicDevice.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}
