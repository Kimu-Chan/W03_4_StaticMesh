#pragma once
#include "Define.h"
#include <d3d11.h>
class UPrimitiveBatch
{
public:
	UPrimitiveBatch();
	~UPrimitiveBatch();
	static UPrimitiveBatch& GetInstance() {
		static UPrimitiveBatch instance;
		return instance;
	}

public:
	void ClearGrid() {};
	float GetSpacing() { return GridParam.gridSpacing; }
	void GenerateGrid(float spacing, int gridCount);
	void RenderBatch(const FMatrix& View, const FMatrix& Projection);
	void AddBoxForCube(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix);
	void AddBoxForSphere(const FVector& center,bool isUniform, FVector radius, const FMatrix& modelMatrix);
	

	void AddCone(const FVector& center, float radius, float height, int segments, const FMatrix& modelMatri);
	
	// 복사 생성자 및 대입 연산자 삭제
	UPrimitiveBatch(const UPrimitiveBatch&) = delete;
	UPrimitiveBatch& operator=(const UPrimitiveBatch&) = delete;
private:
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pBoundingBoxBuffer;
	ID3D11ShaderResourceView* pBoundingBoxSRV;
	ID3D11Buffer* pConesBuffer;
	ID3D11ShaderResourceView* pConesSRV;

	size_t allocatedBoundingBoxCapacity;
	size_t allocatedConeCapacity;
	TArray<FBoundingBox> BoundingBoxes;
	TArray<FCone> Cones;
	FGridParameters GridParam;
	int ConeSegmentCount = 0;
};