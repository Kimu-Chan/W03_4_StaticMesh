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
	if (vertexTextureBuffer)
	{
		vertexTextureBuffer->Release();
		vertexTextureBuffer = nullptr;
	}
	if (indexTextureBuffer)
	{
		indexTextureBuffer->Release();
		indexTextureBuffer = nullptr;
	}
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
	
	TArray<FVector> quad;
	for (int i = 0; i < 4; i++)
	{
		quad.push_back(FVector(quadTextureVertices[i].x, 
			quadTextureVertices[i].y, quadTextureVertices[i].z));
	}
	return CheckPickingOnNDC(quad);

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
	numVertices = sizeof(quadTextureVertices) / sizeof(FVertexTexture);
	numIndices = sizeof(quadTextureInices) / sizeof(uint32);
	vertexTextureBuffer = FEngineLoop::renderer.CreateVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
	indexTextureBuffer = FEngineLoop::renderer.CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices));

	if (!vertexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
	if (!indexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
}

bool UBillboardComponent::CheckPickingOnNDC(const TArray<FVector>& checkQuad)
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
	pickPosition.z = 1.0f; // Near Plane

	FMatrix M = CreateBillboardMatrix();
	FMatrix V = GEngineLoop.View;
	FMatrix P = projectionMatrix;
	FMatrix MVP = M * V * P;

	TArray<FVector4> transformedVerts;
	for (int i = 0; i < checkQuad.size(); i++)
	{
		FVector4 v = FVector4(checkQuad[i].x, checkQuad[i].y, checkQuad[i].z, 1.0f);
		FVector4 clipPos = FMatrix::TransformVector(v, MVP);
		
		if (clipPos.a != 0)
			transformedVerts.push_back((clipPos / clipPos.a));
		else
			transformedVerts.push_back(clipPos);
	}
	float minX = min(min(transformedVerts[0].x, transformedVerts[1].x), min(transformedVerts[2].x, transformedVerts[3].x));
	float maxX = max(max(transformedVerts[0].x, transformedVerts[1].x), max(transformedVerts[2].x, transformedVerts[3].x));
	float minY = min(min(transformedVerts[0].y, transformedVerts[1].y), min(transformedVerts[2].y, transformedVerts[3].y));
	float maxY = max(max(transformedVerts[0].y, transformedVerts[1].y), max(transformedVerts[2].y, transformedVerts[3].y));

	if (pickPosition.x >= minX && pickPosition.x <= maxX &&
		pickPosition.y >= minY && pickPosition.y <= maxY)
	{
		result = true;
	}

	return result;
}
