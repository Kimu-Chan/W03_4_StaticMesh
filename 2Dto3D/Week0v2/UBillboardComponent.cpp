#include "UBillboardComponent.h"
#include "JungleMath.h"
#include "Player.h"
#include "QuadTexture.h"

#include <DirectXMath.h>
UBillboardComponent::UBillboardComponent() : UPrimitiveComponent("Quad")
{
}

UBillboardComponent::UBillboardComponent(FString _Type) : UPrimitiveComponent(_Type)
{
}

UBillboardComponent::~UBillboardComponent()
{
}

void UBillboardComponent::Initialize()
{
    Super::Initialize();
	CreateQuadTextureVertexBuffer();
	m_texture.init();
}

void UBillboardComponent::Update(double deltaTime)
{
    Super::Update(deltaTime);
}

void UBillboardComponent::Release()
{
}

void UBillboardComponent::Render()
{
	FEngineLoop::renderer.PrepareTextureShader();

	if (!GetWorld()->GetPickingObj() || GetWorld()->GetPlayer()->GetControlMode() != CM_TRANSLATION)
	{
		return;
	}

	FMatrix M = CreateBillboardMatrix();
	FMatrix VP = GetEngine().View * GetEngine().Projection;

	// 최종 MVP 행렬
	FMatrix MVP = M * VP;
	if (this == GetWorld()->GetPickingGizmo()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);


	FEngineLoop::renderer.RenderTexturePrimitive(vertexTextureBuffer,numVertices,
		indexTextureBuffer,numIndices,m_texture.m_TextureSRV,m_texture.m_SamplerState);
	//Super::Render();

	FEngineLoop::renderer.PrepareShader();
}

FMatrix UBillboardComponent::CreateBillboardMatrix()
{
	FMatrix CameraView = GetEngine().View;

	CameraView.M[0][3] = 0.0f;
	CameraView.M[1][3] = 0.0f;
	CameraView.M[2][3] = 0.0f;


	CameraView.M[3][0] = 0.0f;
	CameraView.M[3][1] = 0.0f;
	CameraView.M[3][2] = 0.0f;
	CameraView.M[3][3] = 1.0f;


	CameraView.M[0][2] = -CameraView.M[0][2];
	CameraView.M[1][2] = -CameraView.M[1][2];
	CameraView.M[2][2] = -CameraView.M[2][2];
	FMatrix LookAtCamera = FMatrix::Transpose(CameraView);

	FVector worldScale = GetWorldScale();
	FMatrix S = FMatrix::CreateScale(worldScale.x, worldScale.y, worldScale.z);
	FMatrix R = LookAtCamera;
	FMatrix T = FMatrix::CreateTranslationMatrix(GetWorldLocation());
	FMatrix M = S * R * T;

	return M;
}

void UBillboardComponent::CreateQuadTextureVertexBuffer()
{
	uint32 vCount = sizeof(quadTextureVertices)/sizeof(FVertexTexture);
	uint32 iCount = sizeof(quadTextureInices) / sizeof(uint32);
	numVertices = vCount;
	numIndices = iCount;
	vertexTextureBuffer = FEngineLoop::renderer.CreateVertexTextureBuffer(quadTextureVertices, vCount * sizeof(FVertexTexture));
	indexTextureBuffer = FEngineLoop::renderer.CreateIndexBuffer(quadTextureInices, iCount * sizeof(UINT));

	if (!vertexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
	if (!indexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
}
