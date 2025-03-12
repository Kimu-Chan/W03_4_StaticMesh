#include "ArrowComp.h"
#include "JungleMath.h"

UArrowComp::UArrowComp() : UPrimitiveComponent("Arrow")
{
}

UArrowComp::~UArrowComp()
{
}

void UArrowComp::Initialize()
{
	Super::Initialize();
}

void UArrowComp::Update(double deltaTime)
{
	Super::Update(deltaTime);
}

void UArrowComp::Release()
{
}

void UArrowComp::Render()
{
	if (!GetWorld()->GetPickingObj())
		return;
	FMatrix Model = JungleMath::CreateModelMatrix(GetWorldLocation(), GetWorldRotation(), GetWorldScale());

	// 최종 MVP 행렬
	FMatrix MVP = Model * GetEngine().View * GetEngine().Projection;
	if (this == GetWorld()->GetPickingGizmo()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);
	switch (Dir)
	{
	case AD_X:
		FEngineLoop::renderer.RenderPrimitive(FEngineLoop::resourceMgr.vertexBufferArrowX, 
			FEngineLoop::resourceMgr.numVerticesArrowX,
			FEngineLoop::resourceMgr.indiceBufferArrowX,
			FEngineLoop::resourceMgr.numIndicesArrowX
		);
		break;
	case AD_Y:
		//FEngineLoop::renderer.RenderPrimitive(FEngineLoop::resourceMgr.vertexBufferArrowY, FEngineLoop::resourceMgr.numVerticesArrowY);
		FEngineLoop::renderer.RenderPrimitive(FEngineLoop::resourceMgr.vertexBufferArrowY,
			FEngineLoop::resourceMgr.numVerticesArrowY,
			FEngineLoop::resourceMgr.indiceBufferArrowY,
			FEngineLoop::resourceMgr.numIndicesArrowY
		);
		break;
		break;
	case AD_Z:
		//FEngineLoop::renderer.RenderPrimitive(FEngineLoop::resourceMgr.vertexBufferArrowZ, FEngineLoop::resourceMgr.numVerticesArrowZ);
		FEngineLoop::renderer.RenderPrimitive(FEngineLoop::resourceMgr.vertexBufferArrowZ,
			FEngineLoop::resourceMgr.numVerticesArrowZ,
			FEngineLoop::resourceMgr.indiceBufferArrowZ,
			FEngineLoop::resourceMgr.numIndicesArrowZ
		);
		break;
		break;
	case AD_END:
		UE_LOG(LogLevel::Warning, "whatthe...");
		break;
	default:
		break;
	}
}
