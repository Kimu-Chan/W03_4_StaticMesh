#include "UParticleSubUVComp.h"

UParticleSubUVComp::UParticleSubUVComp() :UBillboardComponent("Quad")
{
}

UParticleSubUVComp::~UParticleSubUVComp()
{
}

void UParticleSubUVComp::Initialize()
{

	Super::Initialize();
}

void UParticleSubUVComp::Update(double deltaTime)
{
	//Console::GetInstance().AddLog(LogLevel::Warning, "NumV %d, NumI %d", numVertices,numIndices);
	/*
	
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



	int charIndex = 'A' - ' ';

	float normalWidthOffset = float(CellWidth) / float(BitmapWidth);
	float normalHeightOffset = float(CellHeight) / float(BitmapHeight);

	float u1 = float(indexU) * normalWidthOffset;
	float v1 = float(indexV) * normalHeightOffset;
	float u2 = u1 + normalWidthOffset;
	float v2 = v1 + normalHeightOffset;

	TArray<FVertexTexture> vertices =
	{
		{-1.0f,1.0f,0.0f,u1,v1},
		{ 1.0f,1.0f,0.0f,u2,v1},
		{-1.0f,-1.0f,0.0f,u1,v2},
		{ 1.0f,-1.0f,0.0f,u2,v2}

	};
	UpdateVertexBuffer(vertices);
	*/
	Super::Update(deltaTime);
}

void UParticleSubUVComp::Release()
{
}

void UParticleSubUVComp::Render()
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

	FEngineLoop::renderer.RenderTextPrimitive(vertexTextureBuffer, numVertices,
		m_texture.m_TextureSRV, m_texture.m_SamplerState);
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
