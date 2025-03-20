#include "UText.h"
#include "Engine/Source/Runtime/Engine/Camera/CameraComponent.h"
#include "Engine/Source/Editor/PropertyEditor/ShowFlags.h"
#include "Engine/Source/Runtime/Core/Math/JungleMath.h"
UText::UText() : UBillboardComponent("Quad")
{
}

UText::~UText()
{
	if (vertexTextBuffer)
	{
		vertexTextBuffer->Release();
		vertexTextBuffer = nullptr;
	}
}

void UText::Initialize()
{
	Super::Initialize();
}

void UText::Update(double deltaTime)
{

	Super::Update(deltaTime);

	//FVector newCamera = GetWorld()->GetCamera()->GetForwardVector();
	//newCamera.z = 0;
	//newCamera = newCamera.Normalize();
	//float tmp = FVector(1.0f, 0.0f, 0.0f).Dot(newCamera);
	//float rad = acosf(tmp);
	//float degree = JungleMath::RadToDeg(rad);
	//FVector vtmp = FVector(1.0f, 0.0f, 0.0f).Cross(GetWorld()->GetCamera()->GetForwardVector());
	//if (vtmp.z < 0)
	//	degree *= -1;
	//RelativeRotation.z = degree + 90;
}

void UText::Release()
{
}

void UText::Render()
{
	TextMVPRendering();
}
void UText::ClearText()
{
	vertexTextureArr.clear();
}
void UText::SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn)
{
	RowCount = _cellsPerRow;
	ColumnCount = _cellsPerColumn;
}

int UText::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
	if (!(ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))) {
		return 0;
	}
	/*
	int nIntersections = 0;

	TArray<FVertexSimple> verArr;
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

	for (auto it : vertexTextureArr)
	{
		FVector vtmp = FVector(it.x, it.y, it.z);
		FMatrix::TransformVector(vtmp,LookAtCamera);
		FVertexSimple tmp ;
		tmp.x = vtmp.x ;tmp.y = vtmp.y; tmp.z = vtmp.z;
		//UE_LOG(LogLevel::Warning, "Text x : %f y: %f z : %f", tmp.x, tmp.y, tmp.z);
		verArr.push_back(tmp);
	}
	FVertexSimple* vertices = verArr.data();
	int vCount = verArr.size();
	UINT* indices = nullptr;
	int iCount = 0;

	if (!vertices) return 0;
	BYTE* pbPositions = reinterpret_cast<BYTE*>(vertices);

	int nPrimitives = (!indices) ? (vCount / 3) : (iCount / 3);
	float fNearHitDistance = FLT_MAX;
	for (int i = 0; i < nPrimitives; i++) {
		int idx0, idx1, idx2;
		if (!indices) {
			idx0 = i * 3;
			idx1 = i * 3 + 1;
			idx2 = i * 3 + 2;
		}
		else {
			idx0 = indices[i * 3];
			idx2 = indices[i * 3 + 1];
			idx1 = indices[i * 3 + 2];
		}

		// 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
		uint32 stride = sizeof(FVertexSimple);
		FVector v0 = *reinterpret_cast<FVector*>(pbPositions + idx0 * stride);
		FVector v1 = *reinterpret_cast<FVector*>(pbPositions + idx1 * stride);
		FVector v2 = *reinterpret_cast<FVector*>(pbPositions + idx2 * stride);

		float fHitDistance;
		if (IntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, fHitDistance)) {
			if (fHitDistance < fNearHitDistance) {
				pfNearHitDistance = fNearHitDistance = fHitDistance;
			}
			nIntersections++;
		}

	}
	return nIntersections;
	*/

	for (int i = 0; i < vertexTextureArr.size(); i++)
	{
		quad.push_back(FVector(vertexTextureArr[i].x,
			vertexTextureArr[i].y, vertexTextureArr[i].z));
	}

	return CheckPickingOnNDC(quad, pfNearHitDistance);
}


void UText::SetText(FWString _text)
{
	text = _text;
	if (_text.empty())
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text is empty");

		vertexTextureArr.clear();
		quad.clear();

		// 기존 버텍스 버퍼가 있다면 해제
		if (vertexTextBuffer)
		{
			vertexTextBuffer->Release();
			vertexTextBuffer = nullptr;
		}
		return;
	}
	int textSize = static_cast<int>(_text.size());


	uint32 BitmapWidth = Texture->width;
	uint32 BitmapHeight = Texture->height;

	float CellWidth = float(BitmapWidth) / ColumnCount;
	float CellHeight = float(BitmapHeight) / RowCount;

	float nTexelUOffset = CellWidth / BitmapWidth;
	float nTexelVOffset = CellHeight / BitmapHeight;

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
	UINT byteWidth = static_cast<UINT>(vertexTextureArr.size() * sizeof(FVertexTexture));

	float lastX = -1.0f + quadSize * _text.size();
	quad.push_back(FVector(-1.0f, 1.0f, 0.0f));
	quad.push_back(FVector(-1.0f, -1.0f, 0.0f));
	quad.push_back(FVector(lastX, 1.0f, 0.0f));
	quad.push_back(FVector(lastX, -1.0f, 0.0f));

	CreateTextTextureVertexBuffer(vertexTextureArr, byteWidth);
}
void UText::setStartUV(wchar_t hangul, float& outStartU, float& outStartV)
{
	//대문자만 받는중
	int StartU = 0;
	int StartV = 0;
	int offset = -1;

	if (hangul == L' ') {
		outStartU = 0;  // Space는 특별히 UV 좌표를 (0,0)으로 설정
		outStartV = 0;
		offset = 0;
		return;
	}
	else if (hangul >= L'A' && hangul <= L'Z') {

		StartU = 11;
		StartV = 0;
		offset = hangul - L'A'; // 대문자 위치
	}
	else if (hangul >= L'a' && hangul <= L'z') {
		StartU = 37;
		StartV = 0;
		offset = (hangul - L'a'); // 소문자는 대문자 다음 위치
	}
	else if (hangul >= L'0' && hangul <= L'9') {
		StartU = 1;
		StartV = 0;
		offset = (hangul - L'0'); // 숫자는 소문자 다음 위치
	}
	else if (hangul >= L'가' && hangul <= L'힣')
	{
		StartU = 63;
		StartV = 0;
		offset = hangul - L'가'; // 대문자 위치
	}

	if (offset == -1)
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text Error");
	}

	int offsetV = (offset + StartU) / ColumnCount;
	int offsetU = (offset + StartU) % ColumnCount;

	outStartU = static_cast<float>(offsetU);
	outStartV = static_cast<float>(StartV + offsetV);
}


void UText::setStartUV(char alphabet, float& outStartU, float& outStartV)
{
	//대문자만 받는중
	int StartU = 0;
	int StartV = 0;
	int offset = -1;


	if (alphabet == ' ') {
		outStartU = 0;  // Space는 특별히 UV 좌표를 (0,0)으로 설정
		outStartV = 0;
		offset = 0;
		return;
	}
	else if (alphabet >= 'A' && alphabet <= 'Z') {

		StartU = 1;
		StartV = 4;
		offset = alphabet - 'A'; // 대문자 위치
	}
	else if (alphabet >= 'a' && alphabet <= 'z') {
		StartU = 1;
		StartV = 6;
		offset = (alphabet - 'a'); // 소문자는 대문자 다음 위치
	}
	else if (alphabet >= '0' && alphabet <= '9') {
		StartU = 0;
		StartV = 3;
		offset = (alphabet - '0'); // 숫자는 소문자 다음 위치
	}

	if (offset == -1)
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text Error");
	}

	int offsetV = (offset + StartU) / ColumnCount;
	int offsetU = (offset + StartU) % ColumnCount;

	outStartU = static_cast<float>(offsetU);
	outStartV = static_cast<float>(StartV + offsetV);

}


void UText::CreateTextTextureVertexBuffer(const TArray<FVertexTexture>& _vertex, UINT byteWidth)
{
	numTextVertices = static_cast<UINT>(_vertex.size());
	// 2. Create a vertex buffer
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = byteWidth;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { _vertex.data() };

	ID3D11Buffer* vertexBuffer;

	HRESULT hr = FEngineLoop::graphicDevice.Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
	if (FAILED(hr))
	{
		UE_LOG(LogLevel::Warning, "VertexBuffer Creation faild");
	}
	vertexTextBuffer = vertexBuffer;

	//FEngineLoop::resourceMgr.RegisterMesh(&FEngineLoop::renderer, "JungleText", _vertex, _vertex.size() * sizeof(FVertexTexture),
	//	nullptr, 0);

}

void UText::TextMVPRendering()
{
	FEngineLoop::renderer.PrepareTextureShader();
	//FEngineLoop::renderer.UpdateSubUVConstant(0, 0);
	//FEngineLoop::renderer.PrepareSubUVConstant();

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
		FEngineLoop::renderer.RenderTextPrimitive(vertexTextBuffer, numTextVertices,
			Texture->TextureSRV, Texture->SamplerState);
	}
	//Super::Render();

	FEngineLoop::renderer.PrepareShader();
}


