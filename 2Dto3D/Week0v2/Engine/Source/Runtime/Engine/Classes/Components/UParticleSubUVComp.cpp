#include "Engine/Source/Editor/PropertyEditor/ShowFlags.h"
#include "UParticleSubUVComp.h"

UParticleSubUVComp::UParticleSubUVComp() :UBillboardComponent("Quad")
{
}

UParticleSubUVComp::~UParticleSubUVComp()
{
	if (vertexSubUVBuffer)
	{
		vertexSubUVBuffer->Release();
		vertexSubUVBuffer = nullptr;
	}
}

void UParticleSubUVComp::Initialize()
{
	FEngineLoop::renderer.UpdateSubUVConstant(0, 0);
	FEngineLoop::renderer.PrepareSubUVConstant();
	Super::Initialize();
}

void UParticleSubUVComp::Update(double deltaTime)
{

	uint32 CellWidth = Texture->width / CellsPerColumn;
	uint32 CellHeight = Texture->height / CellsPerColumn;

	static int indexU = 0;
	static int indexV = 0;
	static float second = 0;
	second += static_cast<float>(deltaTime);
	if (second >= 75)
	{
		indexU++;
		second = 0;
	}
	if (indexU >= CellsPerColumn)
	{
		indexU = 0;
		indexV++;
	}
	if (indexV >= CellsPerRow)
	{
		indexU = 0;
		indexV = 0;
		GetWorld()->ThrowAwayObj(this);
		GetWorld()->SetPickingObj(nullptr);
	}


	float normalWidthOffset = float(CellWidth) / float(Texture->width);
	float normalHeightOffset = float(CellHeight) / float(Texture->height);

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
	if (ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_BillboardText)) {

		FEngineLoop::renderer.RenderTexturePrimitive(vertexSubUVBuffer, numTextVertices,
			indexTextureBuffer, numIndices, Texture->TextureSRV, Texture->SamplerState);
	}
	//Super::Render();

	FEngineLoop::renderer.UpdateSubUVConstant(0, 0);
	FEngineLoop::renderer.PrepareSubUVConstant();
	FEngineLoop::renderer.PrepareShader();
}

void UParticleSubUVComp::SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn)
{
	CellsPerRow = _cellsPerRow;
	CellsPerColumn = _cellsPerColumn;

	CreateSubUVVertexBuffer();
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

	uint32 CellWidth = Texture->width / CellsPerColumn;
	uint32 CellHeight = Texture->height / CellsPerColumn;
	float normalWidthOffset = float(CellWidth) / float(Texture->width);
	float normalHeightOffset = float(CellHeight) / float(Texture->height);

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

	vertexSubUVBuffer = FEngineLoop::renderer.CreateVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size() * sizeof(FVertexTexture)));
	numTextVertices = static_cast<UINT>(vertices.size());
}
