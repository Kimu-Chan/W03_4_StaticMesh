#pragma once
#include <Windows.h>
#include "GraphicDevice.h"
#include "Define.h"

class FRenderer;
class FResourceMgr
{
public:
	void Initialize(FRenderer* renderer);
	void Release(FRenderer* renderer);
	void LoadObjAsset(FRenderer* renderer, FWString filepath, ID3D11Buffer** vertexBuffer, UINT& numVertices, ID3D11Buffer** indexBuffer, UINT& numIndices);
public:
	ID3D11Buffer* vertexBufferSphere = nullptr;
	
	ID3D11Buffer* vertexBufferCube = nullptr;
	ID3D11Buffer* indiceBufferCube = nullptr;

	ID3D11Buffer* vertexBufferGizmo = nullptr;
	
	ID3D11Buffer* vertexBufferArrowX = nullptr;
	ID3D11Buffer* indiceBufferArrowX = nullptr;

	ID3D11Buffer* vertexBufferArrowY = nullptr;
	ID3D11Buffer* vertexBufferArrowZ = nullptr;


	UINT numVerticesSphere;
	
	UINT numVerticesCube;
	UINT numIndicesCube;

	UINT numVerticesGizmo;
	
	UINT numVerticesArrowX;
	UINT numIndicesArrowX;

	UINT numVerticesArrowY;
	UINT numVerticesArrowZ;
};

