#include "ResourceMgr.h"
#include <fstream>
#include <sstream>
#include "Sphere.h"
#include "Cube.h"
#include "Gizmo.h"
#include "Renderer.h"
#include "Define.h"
#include "Arrow.h"
void FResourceMgr::Initialize(FRenderer* renderer)
{
    RegisterMesh(renderer, "Sphere", sphere_vertices, sizeof(sphere_vertices) / sizeof(FVertexSimple), nullptr, 0);
    RegisterMesh(renderer, "Cube", cube_vertices, sizeof(cube_vertices) / sizeof(FVertexSimple), cube_indices, sizeof(cube_indices) / sizeof(uint32));
    RegisterMesh(renderer, "Gizmo", gizmoVertices, sizeof(gizmoVertices) / sizeof(FVertexSimple), nullptr, 0);

    LoadObjAsset(renderer, "ArrowX", L"Assets/AxisArrow.obj");
    LoadObjAsset(renderer, "ArrowY", L"Assets/AxisArrow.obj");
    LoadObjAsset(renderer, "ArrowZ", L"Assets/AxisArrow.obj");
}

void FResourceMgr::Release(FRenderer* renderer) {
    for (auto& pair : meshMap) {
        FStaticMesh* mesh = pair.second.get();
        renderer->ReleaseBuffer(mesh->vertexBuffer);
        renderer->ReleaseBuffer(mesh->indexBuffer);
    }
}

void FResourceMgr::LoadObjAsset(FRenderer* renderer, const FString& meshName, const FWString& filepath)
{
	std::ifstream objFile(filepath.c_str());
	TArray<FVector> positions;
	TArray<FVertexSimple> vertices;
	TArray<uint32> indices;

	if (objFile) {

		FString line;

		while (std::getline(objFile, line)) {
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;

			if (type == "v") // Vertex position
			{
				FVector vertex;
				FVector color;
				lineStream >> vertex.x >> vertex.y >> vertex.z >> color.x >> color.y >> color.z;

				FVertexSimple vertexSimple{ vertex.x, vertex.y, vertex.z, color.x, color.y , color.z, 1 };
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


std::shared_ptr<FStaticMesh> FResourceMgr::GetMesh(const FString& name) const
{
    auto it = meshMap.find(name);
    return (it != meshMap.end()) ? it->second : nullptr;
}
