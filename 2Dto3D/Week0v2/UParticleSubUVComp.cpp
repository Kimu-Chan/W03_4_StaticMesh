#include "UParticleSubUVComp.h"

UParticleSubUVComp::UParticleSubUVComp() :UBillboardComponent("Quad")
{
}

UParticleSubUVComp::~UParticleSubUVComp()
{
}

void UParticleSubUVComp::Initialize()
{
	FEngineLoop::renderer.UpdateSubUVConstant(0, 0);
	FEngineLoop::renderer.PrepareSubUVConstant();
	CreateSubUVVertexBuffer();
	Super::Initialize();
}

void UParticleSubUVComp::Update(double deltaTime)
{
	//Console::GetInstance().AddLog(LogLevel::Warning, "NumV %d, NumI %d", numVertices,numIndices);
	
	
	static int indexU = 0;
	static int indexV = 0;
	static float second = 0;
	second += deltaTime;
	if (second >= 75)
	{
		indexU++;
		second = 0;
	}
	if (indexU >= 6)
	{
		indexU = 0;
		indexV++;
	}
	if (indexU >= 6 && indexV >= 6)
	{

		indexU = 0;
		indexV = 0;
	}

	float normalWidthOffset = float(CellWidth) / float(BitmapWidth);
	float normalHeightOffset = float(CellHeight) / float(BitmapHeight);

	float u1 = float(indexU) * normalWidthOffset;
	float v1 = float(indexV) * normalHeightOffset;
	float u2 = u1 + normalWidthOffset;
	float v2 = v1 + normalHeightOffset;

	finalIndexU = float(indexU) * normalWidthOffset;
	finalIndexV = float(indexV) * normalHeightOffset;
	

	Super::Update(deltaTime);
}

void UParticleSubUVComp::Release()
{
}

void UParticleSubUVComp::Render()
{
	FEngineLoop::renderer.PrepareTextureShader();
	FEngineLoop::renderer.UpdateSubUVConstant(finalIndexU, finalIndexV);
	FEngineLoop::renderer.PrepareSubUVConstant();

	FMatrix M = CreateBillboardMatrix();
	FMatrix VP = GetEngine().View * GetEngine().Projection;

	// 최종 MVP 행렬
	FMatrix MVP = M * VP;
	if (this == GetWorld()->GetPickingGizmo()) {
		FEngineLoop::renderer.UpdateConstant(MVP, 1.0f);
	}
	else
		FEngineLoop::renderer.UpdateConstant(MVP, 0.0f);

	FEngineLoop::renderer.RenderTexturePrimitive(vertexSubUVBuffer, numTextVertices,
		indexTextureBuffer, numIndices, m_texture.m_TextureSRV, m_texture.m_SamplerState);
	//Super::Render();

	FEngineLoop::renderer.PrepareShader();
}

void UParticleSubUVComp::UpdateVertexBuffer(const TArray<FVertexTexture>& vertices)
{
	/*
	ID3D11DeviceContext* context = FEngineLoop::graphicDevice.DeviceContext;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(vertexTextureBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices.data(), vertices.size() * sizeof(FVertexTexture));
	context->Unmap(vertexTextureBuffer, 0);
	*/

}

void UParticleSubUVComp::CreateSubUVVertexBuffer()
{

	float normalWidthOffset = float(CellWidth) / float(BitmapWidth);
	float normalHeightOffset = float(CellHeight) / float(BitmapHeight);

	TArray<FVertexTexture> vertices =
	{
		{-1.0f,1.0f,0.0f,0,0},
		{ 1.0f,1.0f,0.0f,1,0},
		{-1.0f,-1.0f,0.0f,0,1},
		{ 1.0f,-1.0f,0.0f,1,1}

	};
	vertices[1].u = normalWidthOffset;
	vertices[2].v = normalHeightOffset;
	vertices[3].u = normalWidthOffset;
	vertices[3].v = normalHeightOffset;

	vertexSubUVBuffer = FEngineLoop::renderer.CreateVertexBuffer(vertices.data(), vertices.size() * sizeof(FVertexTexture));
	numTextVertices = vertices.size();
}
