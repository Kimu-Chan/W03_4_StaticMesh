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

	static int index = 0;
	static float second = 0;
	second += deltaTime;
	if (second >= 75)
	{
		index ++;
		second = 0;
	}
	if (index >= 16 ) index = 0;


	int charIndex = 'A' - ' ';

	float normalWidthOffset = float(CellWidth) / float(BitmapWidth);
	float normalHeightOffset = float(CellHeight) / float(BitmapHeight);

	float u1 = float(index)*normalWidthOffset;
	float v1 = float(index)*normalHeightOffset;
	float u2 = u1+normalWidthOffset;
	float v2 = v1+normalHeightOffset;

	TArray<FVertexTexture> vertices =
	{
		{-1.0f,1.0f,0.0f,u1,v1},
		{ 1.0f,1.0f,0.0f,u2,v1},
		{-1.0f,-1.0f,0.0f,u1,v2},
		{ 1.0f,-1.0f,0.0f,u2,v2}
	};


	UpdateVertexBuffer(vertices);


    Super::Update(deltaTime);
}

void UBillboardComponent::Release()
{
}

void UBillboardComponent::Render()
{
	FEngineLoop::renderer.PrepareTextureShader();

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

void UBillboardComponent::UpdateVertexBuffer(const TArray<FVertexTexture>& vertices)
{
	ID3D11DeviceContext* context = FEngineLoop::graphicDevice.DeviceContext;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(vertexTextureBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices.data(), vertices.size() * sizeof(FVertexTexture));
	context->Unmap(vertexTextureBuffer, 0);

}
