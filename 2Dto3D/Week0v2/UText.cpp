#include "UText.h"

UText::UText() : UBillboardComponent("Quad")
{
}

UText::~UText()
{
}

void UText::Initialize()
{

	Super::Initialize();
}

void UText::Update(double deltaTime)
{

	Super::Update(deltaTime);
}

void UText::Release()
{
}

void UText::Render()
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
	
	FEngineLoop::renderer.RenderTextPrimitive(vertexTextBuffer, numVertices,
		m_texture.m_TextureSRV, m_texture.m_SamplerState);
	//Super::Render();

	FEngineLoop::renderer.PrepareShader();
}

void UText::SetText(FWString _text)
{
	if (_text.empty())
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text is empty");
	}

	TArray<FVertexTexture> vertexTextureArr;
	int textSize = _text.size();

	float nTexelUOffset = CellWidth / BitmapWidth;
	float nTexelVOffset = CellHeight/ BitmapHeight;

	for (int i = 0; i < _text.size(); i++)
	{
		FVertexTexture leftUP = { -1.0f,1.0f,0.0f,0.0f,0.0f };
		FVertexTexture rightUP = { 1.0f,1.0f,0.0f,1.0f,0.0f };
		FVertexTexture leftDown = { -1.0f,-1.0f,0.0f,0.0f,1.0f };
		FVertexTexture rightDown = { 1.0f,-1.0f,0.0f,1.0f,1.0f };
		rightUP.u *= nTexelUOffset;
		leftDown.v *= nTexelVOffset;
		rightDown.u *= nTexelUOffset;
		rightDown.v *= nTexelVOffset;

		leftUP.x += quadWidth * i;
		rightUP.x += quadWidth * i;
		leftDown.x += quadWidth * i;
		rightDown.x += quadWidth * i;

		float startU = 0.0f;
		float startV = 0.0f;

		setStartUV(_text[i], startU, startV);
		leftUP.u += (nTexelUOffset * startU);
		leftUP.v += (nTexelVOffset * startV);
		rightUP.u += (nTexelUOffset * startU);
		rightUP.v += (nTexelVOffset * startV);
		leftDown.u += (nTexelUOffset * startU);
		leftDown.v += (nTexelVOffset * startV);
		rightDown.u += (nTexelUOffset * startU);
		rightDown.v += (nTexelVOffset * startV);

		vertexTextureArr.push_back(leftUP);
		vertexTextureArr.push_back(rightUP);
		vertexTextureArr.push_back(leftDown);
		vertexTextureArr.push_back(rightUP);
		vertexTextureArr.push_back(rightDown);
		vertexTextureArr.push_back(leftDown);

	}
	UINT byteWidth = vertexTextureArr.size() * sizeof(FVertexTexture);

	CreateTextTextureVertexBuffer(vertexTextureArr,byteWidth);

}

void UText::setStartUV(char alphabet, float& outStartU, float& outStartV)
{
	//대문자만 받는중
	int aStartU = 1;
	int aStartV = 4;
	int offset = alphabet - 'A';
	int offsetV = (offset+aStartU) / ColumnCount;
	int offsetU = (offset+aStartU) % ColumnCount;

	outStartU = offsetU;
	outStartV = aStartV + offsetV;

}

void UText::CreateTextTextureVertexBuffer(const TArray<FVertexTexture>& _vertex,UINT byteWidth)
{
	numVertices = _vertex.size();
	// 2. Create a vertex buffer
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = byteWidth;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { _vertex.data()};

	ID3D11Buffer* vertexBuffer;
	
	HRESULT hr = FEngineLoop::graphicDevice.Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
	if (FAILED(hr))
	{
		UE_LOG(LogLevel::Warning, "VertexBuffer Creation faild");
	}
	vertexTextBuffer = vertexBuffer;
}


