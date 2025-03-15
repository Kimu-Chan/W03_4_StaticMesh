#pragma once
#include "Define.h"

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
	void AddLine(const FVector& start, const FVector& end, const FVector4& color);
	void AddGridLine(const FVector& start, const FVector& end, const FVector4& color);
	void AddGrid(int gridSize);
	void ClearGrid();
	void Begin();
	void SetSpacing(float spacing);
	float GetSpacing() { return Spacing; }
	void End(const FMatrix& View, const FMatrix& Projection);
	void AddBoxForCube(const FVector& center, const FVector4& color, const FMatrix& modelMatrix);
	void AddBox(const FVector& minPoint, const FVector& maxPoint, const FVector4& color);
	void AddBoxForSphere(const FVector& center,bool isUniform, float radius, const FMatrix& modelMatrix, const FVector4& color);
	

	void AddCone(const FVector& center, float radius, float height, int segments, const FVector4& color, const FMatrix& modelMatri);
	void AddWorldGizmo();
	
	// 복사 생성자 및 대입 연산자 삭제
	UPrimitiveBatch(const UPrimitiveBatch&) = delete;
	UPrimitiveBatch& operator=(const UPrimitiveBatch&) = delete;
private:
	ID3D11Buffer* vertexBuffer;
	size_t allocatedCapacity;
	TArray<FVertexSimple> Vertices;
	TArray<FVertexSimple> GridVertices;
	float Spacing = 5.f;
	float PreSpacing = 0.f;
	
};