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
	TArray<FVertexTexture> vertices;
	TArray<uint32> indices;
	uint32 indexOffset = 0;

	int charIndex = 'A' - ' ';
	int col = charIndex % 16;
	int row = charIndex / 16;

	float u1 = 5 * (32.0f / 512.0f);
	float v1 = 2 * (32.0f / 512.0f);
	float u2 = u1 + (32.0f / 512.0f);
	float v2 = v1 + (32.0f / 512.0f);

	uint32 localQuadTextureInices[] =
	{
		0,1,2,
		1,3,2
	};

	FVertexTexture localQuadTextureVertices[] =
	{
		{-1.0f,1.0f,0.0f,u1,v1},
		{ 1.0f,1.0f,0.0f,u2,v1},
		{-1.0f,-1.0f,0.0f,u1,v2},
		{ 1.0f,-1.0f,0.0f,u2,v2}
	};

	vertices.push_back(localQuadTextureVertices[0]);
	vertices.push_back(localQuadTextureVertices[1]);
	vertices.push_back(localQuadTextureVertices[2]);
	vertices.push_back(localQuadTextureVertices[3]);

	indices.push_back(localQuadTextureInices[0]);
	indices.push_back(localQuadTextureInices[1]);
	indices.push_back(localQuadTextureInices[2]);
	indices.push_back(localQuadTextureInices[3]);
	indices.push_back(localQuadTextureInices[4]);
	indices.push_back(localQuadTextureInices[5]);

	UpdateVertexBuffer(vertices, indices);


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
	indexTextureBuffer = FEngineLoop::renderer.CreateIndexTextureBuffer(quadTextureInices, iCount * sizeof(UINT));

	if (!vertexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
	if (!indexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
}

void UBillboardComponent::UpdateVertexBuffer(const TArray<FVertexTexture>& vertices, const TArray<uint32>& indices)
{
	ID3D11DeviceContext* context = FEngineLoop::graphicDevice.DeviceContext;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(vertexTextureBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices.data(), vertices.size() * sizeof(FVertexTexture));
	context->Unmap(vertexTextureBuffer, 0);

	context->Map(indexTextureBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, indices.data(), indices.size() * sizeof(uint32));
	context->Unmap(indexTextureBuffer, 0);
}
