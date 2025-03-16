#include "UBillboardComponent.h"
#include "JungleMath.h"
#include "Player.h"
#include "QuadTexture.h"

#include <DirectXMath.h>

UBillboardComponent::UBillboardComponent() : 
	UPrimitiveComponent("Quad")
{
}

UBillboardComponent::UBillboardComponent(FString _Type)
	: UPrimitiveComponent(_Type)
{
}

UBillboardComponent::~UBillboardComponent()
{
}

void UBillboardComponent::Initialize()
{
    Super::Initialize();
	CreateQuadTextureVertexBuffer();
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


	FEngineLoop::renderer.RenderTexturePrimitive(vertexTextureBuffer,numVertices,
		indexTextureBuffer,numIndices,m_texture.m_TextureSRV,m_texture.m_SamplerState);
	//Super::Render();

	FEngineLoop::renderer.PrepareShader();
}


int UBillboardComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{

	/*
	FMatrix worldMatrix = CreateBillboardMatrix();
	FMatrix ViewMatrix = GEngineLoop.View;
	FMatrix inverseMatrix = FMatrix::Inverse(worldMatrix * ViewMatrix);

	FVector cameraOrigin = { 0,0,0 };
	FVector pickRayOrigin = inverseMatrix.TransformPosition(cameraOrigin);


	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(GEngineLoop.hWnd, &mousePos);

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	FEngineLoop::graphicDevice.DeviceContext->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	FVector pickPosition;
	int screenX = mousePos.x;
	int screenY = mousePos.y;
	FMatrix projectionMatrix = GetEngine().Projection;
	pickPosition.x = ((2.0f * screenX / viewport.Width) - 1) / projectionMatrix[0][0];
	pickPosition.y = -((2.0f * screenY / viewport.Height) - 1) / projectionMatrix[1][1];
	pickPosition.z = 1.0f; // Near Plane

	FVector _rayDirection = inverseMatrix.TransformPosition(pickPosition);
	_rayDirection = (_rayDirection - pickRayOrigin).Normalize(); // local 좌표축의 ray

	return Super::CheckRayIntersection(pickRayOrigin, _rayDirection, pfNearHitDistance);
	*/
	return CheckPickingOnNDC();
}




void UBillboardComponent::SetTexture(FWString _fileName)
{
	m_texture.init(_fileName);
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
	vertexTextureBuffer = FEngineLoop::renderer.CreateVertexBuffer(quadTextureVertices, vCount * sizeof(FVertexTexture));
	indexTextureBuffer = FEngineLoop::renderer.CreateIndexBuffer(quadTextureInices, iCount * sizeof(UINT));

	if (!vertexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
	if (!indexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
}

bool UBillboardComponent::CheckPickingOnNDC()
{
	bool result = false;
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(GEngineLoop.hWnd, &mousePos);

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	FEngineLoop::graphicDevice.DeviceContext->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	FVector pickPosition;
	int screenX = mousePos.x;
	int screenY = mousePos.y;
	FMatrix projectionMatrix = GetEngine().Projection;
	pickPosition.x = ((2.0f * screenX / viewport.Width) - 1);
	pickPosition.y = -((2.0f * screenY / viewport.Height) - 1);
	//pickPosition.y = ((2.0f * (viewport.Height - screenY) / viewport.Height) - 1);
	pickPosition.z = 1.0f; // Near Plane

	FMatrix M = CreateBillboardMatrix();
	FMatrix V = GEngineLoop.View;
	FMatrix P = projectionMatrix;
	FMatrix MVP = M * V * P;

	FVector4 transformedVerts[4];
	//TODO 하드코딩
	for (int i = 0; i < 4; i++)
	{
		FVector4 v = FVector4(quadTextureVertices[i].x, quadTextureVertices[i].y, quadTextureVertices[i].z, 1.0f);
		FVector4 clipPos = FMatrix::TransformVector(v, MVP);
		UE_LOG(LogLevel::Display, "ClipPos %f, %f, %f, %f", clipPos.x, clipPos.y, clipPos.z, clipPos.a);

		if (clipPos.a != 0)
			transformedVerts[i] = clipPos / clipPos.a;
		else
			transformedVerts[i] = clipPos;
	}
	float yCorrection = 1.0f / transformedVerts[0].a;
	float minX = min(min(transformedVerts[0].x, transformedVerts[1].x), min(transformedVerts[2].x, transformedVerts[3].x));
	float maxX = max(max(transformedVerts[0].x, transformedVerts[1].x), max(transformedVerts[2].x, transformedVerts[3].x));
	float minY = min(min(transformedVerts[0].y, transformedVerts[1].y), min(transformedVerts[2].y, transformedVerts[3].y));
	float maxY = max(max(transformedVerts[0].y, transformedVerts[1].y), max(transformedVerts[2].y, transformedVerts[3].y));

	//minY *= yCorrection;
	//maxY *= yCorrection;

	for (int i = 0; i < 4; i++)
	{
		UE_LOG(LogLevel::Display, "Quad %f, %f, %f, %f",
		transformedVerts[i].x, transformedVerts[i].y, transformedVerts[i].z, transformedVerts[i].a);
	}
	UE_LOG(LogLevel::Display, "My Click %f, %f, %f", pickPosition.x, pickPosition.y, pickPosition.z);
	
	if (pickPosition.x >= minX && pickPosition.x <= maxX &&
		pickPosition.y >= minY && pickPosition.y <= maxY)
	{
		result = true;
	}

	return result;
}
