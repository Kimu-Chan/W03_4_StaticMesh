#include "ResourceMgr.h"
#include <fstream>
#include <sstream>
#include "Sphere.h"
#include "Cube.h"
#include "Gizmo.h"
#include "Renderer.h"
#include "Define.h"
#include "Arrow.h"
#include "Quad.h"
void FResourceMgr::Initialize(FRenderer* renderer)
{
    RegisterMesh(renderer, "Sphere", sphere_vertices, sizeof(sphere_vertices) / sizeof(FVertexSimple), nullptr, 0);
    RegisterMesh(renderer, "Cube", cube_vertices, sizeof(cube_vertices) / sizeof(FVertexSimple), cube_indices, sizeof(cube_indices) / sizeof(uint32));
    RegisterMesh(renderer, "Gizmo", gizmoVertices, sizeof(gizmoVertices) / sizeof(FVertexSimple), nullptr, 0);
    RegisterMesh(renderer, "Quad", quadVertices, sizeof(quadVertices) / sizeof(FVertexSimple), quadInices, sizeof(quadInices)/sizeof(uint32));

    LoadObjAsset(renderer, "ArrowX", L"Assets/AxisArrowX.obj");
    LoadObjAsset(renderer, "ArrowY", L"Assets/AxisArrowY.obj");
    LoadObjAsset(renderer, "ArrowZ", L"Assets/AxisArrowZ.obj");
	LoadObjAsset(renderer, "ScaleX", L"Assets/AxisScaleArrowX.obj");
	LoadObjAsset(renderer, "ScaleY", L"Assets/AxisScaleArrowY.obj");
	LoadObjAsset(renderer, "ScaleZ", L"Assets/AxisScaleArrowZ.obj");
	LoadObjNormalAsset(renderer, "Cube1", L"Assets/Cube.obj");
}

void FResourceMgr::Release(FRenderer* renderer) {
    for (auto& pair : meshMap) {
        FStaticMesh* mesh = pair.second.get();
        renderer->ReleaseBuffer(mesh->vertexBuffer);
        renderer->ReleaseBuffer(mesh->indexBuffer);
    }
}

void FResourceMgr::LoadObjNormalAsset(FRenderer* renderer, const FString& meshName, const FWString& filepath)
{
	std::ifstream objFile(filepath.c_str());

	TArray<FVector> positions;   // 버텍스 좌표 (v)
	TArray<FVector> normals;     // 노말 데이터 (vn)
	TArray<FVertexSimple> vertices;  // 최종적으로 저장될 버텍스 리스트 (Position + Normal)
	TArray<uint32> indices;      // 인덱스 리스트
	TArray<FVector4> Colors;
	if (objFile)
	{
		FString line;
		while (std::getline(objFile, line))
		{
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;

			if (type == "v") // 버텍스 좌표
			{
				FVector vertex;
				FVector color;
				lineStream >> vertex.x >> vertex.y >> vertex.z >> color.x >> color.y >> color.z;

				positions.push_back(vertex);
				Colors.push_back(FVector4(color.x, color.y, color.z, 1.0f));
			}
			else if (type == "vn") // 노말 데이터
			{
				FVector normal;
				lineStream >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f") // 페이스 (v//vn)
			{
				std::vector<uint32> faceIndices;
				std::vector<uint32> normalIndices;
				std::string vertexInfo;

				while (lineStream >> vertexInfo)
				{
					size_t firstSlash = vertexInfo.find("//");  // v//vn 형식 찾기
					if (firstSlash != std::string::npos)
					{
						int vIdx = std::stoi(vertexInfo.substr(0, firstSlash)) - 1;
						int nIdx = std::stoi(vertexInfo.substr(firstSlash + 2)) - 1;

						faceIndices.push_back(vIdx);
						normalIndices.push_back(nIdx);
					}
				}

				// 삼각형 인덱스 생성
				for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
				{
					indices.push_back(faceIndices[0]);
					indices.push_back(faceIndices[i]);
					indices.push_back(faceIndices[i + 1]);

					// 해당 버텍스들의 노말을 설정
					for (size_t j = 0; j < 3; j++)
					{
						FVector position = positions[faceIndices[j]];
						FVector normal = normals[normalIndices[j]];
						FVector4 color = Colors[faceIndices[j]];
						FVertexSimple vertexSimple{
							position.x, position.y, position.z, color.x, color.y,color.z, color.a,
							normal.x, normal.y, normal.z
						};

						vertices.push_back(vertexSimple);
					}
				}
			}
		}
	}
	for (auto iter : vertices)
	{
		UE_LOG(LogLevel::Display, "%f %f %f %f %f %f %f %f %f %f ", iter.x, iter.y, iter.z, iter.r, iter.g, iter.b, iter.a, iter.nx, iter.ny, iter.nz);
	}
	if (vertices.empty()) {
		UE_LOG(LogLevel::Error, "Error: OBJ file is empty or failed to parse!");
		return;
	}

	// **버텍스 및 인덱스 데이터를 배열로 변환**
	FVertexSimple* vertexArray = new FVertexSimple[vertices.size()];
	std::memcpy(vertexArray, vertices.data(), vertices.size() * sizeof(FVertexSimple));

	UINT* indexArray = nullptr;
	if (!indices.empty()) {
		indexArray = new UINT[indices.size()];
		std::memcpy(indexArray, indices.data(), indices.size() * sizeof(UINT));
	}


	UE_LOG(LogLevel::Error, "Arrow Vertex Size : %d", vertices.size());
	ID3D11Buffer* vertexBuffer = renderer->CreateVertexBuffer(vertexArray, vertices.size() * sizeof(FVertexSimple));
	ID3D11Buffer* indexBuffer = (indexArray) ? renderer->CreateIndexBuffer(indexArray, indices.size() * sizeof(UINT)) : nullptr;
	if (!vertexBuffer) {
		UE_LOG(LogLevel::Error, "Error: Failed to create buffers for OBJ: %s", filepath.c_str());
		delete[] vertexArray;
		delete[] indexArray;
		return;
	}
	meshMap[meshName] = std::make_shared<FStaticMesh>(vertexBuffer, vertices.size(), vertexArray, indexBuffer, indices.size(), indexArray);

	delete[] vertexArray;
	delete[] indexArray;

	UE_LOG(LogLevel::Error, "OBJ Loaded: %s - %d vertices, %d indices", meshName.c_str(), vertices.size(), indices.size());
}


void FResourceMgr::LoadObjAsset(FRenderer* renderer, const FString& meshName, const FWString& filepath)
{
	std::ifstream objFile(filepath.c_str());
	TArray<FVector> positions;
	TArray<FVertexSimple> vertices;
	TArray<uint32> indices;

	if (objFile) {

		FString line;

		int normalcount = 0;
		while (std::getline(objFile, line)) {
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;
			if (type == "v") // Vertex position
			{
				FVector vertex;
				FVector color;
				lineStream >> vertex.x >> vertex.y >> vertex.z >> color.x >> color.y >> color.z;

				FVertexSimple vertexSimple{ vertex.x, vertex.y, vertex.z, color.x, color.y , color.z, 0.0f,0.0f,0.0f };
				vertices.push_back(vertexSimple);
				positions.push_back(vertex);
			}

			else if (type == "f") // Face
			{
				std::vector<uint32_t> faceIndices;
				uint32_t index;

				while (lineStream >> index) {
					faceIndices.push_back(index - 1);
				}

				for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
					indices.push_back(faceIndices[0]);    // ù ��° ����
					indices.push_back(faceIndices[i]);    // �� ��° ����
					indices.push_back(faceIndices[i + 1]); // �� ��° ����
				}
			}

		}
	}
	if (vertices.empty()) {
		UE_LOG(LogLevel::Error, "Error: OBJ file is empty or failed to parse!");
		return;
	}

	// **버텍스 및 인덱스 데이터를 배열로 변환**
	FVertexSimple* vertexArray = new FVertexSimple[vertices.size()];
	std::memcpy(vertexArray, vertices.data(), vertices.size() * sizeof(FVertexSimple));

	UINT* indexArray = nullptr;
	if (!indices.empty()) {
		indexArray = new UINT[indices.size()];
		std::memcpy(indexArray, indices.data(), indices.size() * sizeof(UINT));
	}


	UE_LOG(LogLevel::Error, "Arrow Vertex Size : %d", vertices.size());
	ID3D11Buffer* vertexBuffer = renderer->CreateVertexBuffer(vertexArray, vertices.size() * sizeof(FVertexSimple));
	ID3D11Buffer* indexBuffer = (indexArray) ? renderer->CreateIndexBuffer(indexArray, indices.size() * sizeof(UINT)) : nullptr;
	if (!vertexBuffer) {
		UE_LOG(LogLevel::Error, "Error: Failed to create buffers for OBJ: %s", filepath.c_str());
		delete[] vertexArray;
		delete[] indexArray;
		return;
	}
	meshMap[meshName] = std::make_shared<FStaticMesh>(vertexBuffer, vertices.size(), vertexArray, indexBuffer, indices.size(), indexArray);

	delete[] vertexArray;
	delete[] indexArray;

	UE_LOG(LogLevel::Error, "OBJ Loaded: %s - %d vertices, %d indices", meshName.c_str(), vertices.size(), indices.size());
}


void FResourceMgr::RegisterMesh(FRenderer* renderer, const std::string& name, FVertexSimple* vertices, uint32 vCount, uint32* indices, uint32 iCount)
{
    INT numVertices = vCount;
    UINT numIndices = iCount;

    ID3D11Buffer* vertexBuffer = renderer->CreateVertexBuffer(vertices, vCount * sizeof(FVertexSimple));
    ID3D11Buffer* indexBuffer = (indices && iCount > 0) ? renderer->CreateIndexBuffer(indices, iCount * sizeof(UINT)) : nullptr;
    meshMap[name] = std::make_shared<FStaticMesh>(vertexBuffer, vCount, vertices, indexBuffer, iCount, indices);
}

void FResourceMgr::RegisterMesh(FRenderer* renderer, const std::string& name, TArray<FVertexSimple>& vertices, uint32 vCount, TArray<uint32>& indices, uint32 iCount)
{
	INT numVertices = vCount;
	UINT numIndices = iCount;

	FVertexSimple* vertexArray = new FVertexSimple[vertices.size()];
	std::memcpy(vertexArray, vertices.data(), vertices.size() * sizeof(FVertexSimple));

	UINT* indexArray = nullptr;
	if (!indices.empty()) {
		indexArray = new UINT[indices.size()];
		std::memcpy(indexArray, indices.data(), indices.size() * sizeof(UINT));
	}

	ID3D11Buffer* vertexBuffer = renderer->CreateVertexBuffer(vertices, vCount * sizeof(FVertexSimple));
	ID3D11Buffer* indexBuffer = (!indices.empty() && iCount > 0) ? renderer->CreateIndexBuffer(indices, iCount * sizeof(UINT)) : nullptr;
	meshMap[name] = std::make_shared<FStaticMesh>(vertexBuffer, vCount, vertexArray, indexBuffer, iCount, indexArray);
}

std::shared_ptr<FStaticMesh> FResourceMgr::GetMesh(const FString& name) const
{
    auto it = meshMap.find(name);
    return (it != meshMap.end()) ? it->second : nullptr;
}
