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
	//GenerateSphere();
	//RegisterMesh(renderer, "Sphere", sphere_vertices, sizeof(sphere_vertices) / sizeof(FVertexSimple), nullptr, 0);
	RegisterMesh(renderer, "Cube", cube_vertices, sizeof(cube_vertices) / sizeof(FVertexSimple), cube_indices, sizeof(cube_indices) / sizeof(uint32));
    RegisterMesh(renderer, "Gizmo", gizmoVertices, sizeof(gizmoVertices) / sizeof(FVertexSimple), nullptr, 0);
    RegisterMesh(renderer, "Quad", quadVertices, sizeof(quadVertices) / sizeof(FVertexSimple), quadInices, sizeof(quadInices)/sizeof(uint32));

    LoadObjAsset(renderer, "ArrowX", L"Assets/AxisArrowX.obj");
    LoadObjAsset(renderer, "ArrowY", L"Assets/AxisArrowY.obj");
    LoadObjAsset(renderer, "ArrowZ", L"Assets/AxisArrowZ.obj");
	LoadObjAsset(renderer, "ScaleX", L"Assets/AxisScaleArrowX.obj");
	LoadObjAsset(renderer, "ScaleY", L"Assets/AxisScaleArrowY.obj");
	LoadObjAsset(renderer, "ScaleZ", L"Assets/AxisScaleArrowZ.obj");
	LoadObjNormalAsset(renderer, "Cube", L"Assets/Cube.obj");
	LoadObjNormalAsset(renderer, "Sphere", L"Assets/Sphere.obj");
}

void FResourceMgr::Release(FRenderer* renderer) {
    for (auto& pair : meshMap) {
        FStaticMesh* mesh = pair.second.get();
        renderer->ReleaseBuffer(mesh->vertexBuffer);
        renderer->ReleaseBuffer(mesh->indexBuffer);
    }
}

#include <unordered_map>

struct PairHash {
	template <typename T1, typename T2>
	std::size_t operator()(const std::pair<T1, T2>& pair) const {
		return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
	}
};

void FResourceMgr::LoadObjNormalAsset(FRenderer* renderer, const FString& meshName, const FWString& filepath)
{
	std::ifstream objFile(filepath.c_str());

	TArray<FVector> positions;
	TArray<FVector> normals;
	TArray<FVertexSimple> vertices;
	TArray<uint32> indices;
	TArray<FVector4> Colors;

	if (objFile)
	{
		FString line;
		while (std::getline(objFile, line))
		{
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;

			if (type == "v")
			{
				FVector vertex;
				FVector color;
				lineStream >> vertex.x >> vertex.y >> vertex.z >> color.x >> color.y >> color.z;

				positions.push_back(vertex);
				Colors.push_back(FVector4(color.x,color.y,color.z, 1.0f));
			}
			else if (type == "vn")
			{
				FVector normal;
				lineStream >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f")
			{
				std::vector<uint32> faceIndices;
				std::vector<uint32> normalIndices;
				std::string vertexInfo;

				while (lineStream >> vertexInfo)
				{
					size_t firstSlash = vertexInfo.find("//");
					if (firstSlash != std::string::npos)
					{
						int vIdx = std::stoi(vertexInfo.substr(0, firstSlash)) - 1;
						int nIdx = std::stoi(vertexInfo.substr(firstSlash + 2)) - 1;

						faceIndices.push_back(vIdx);
						normalIndices.push_back(nIdx);
					}
				}

				std::unordered_map<std::pair<int, int>, uint32_t, PairHash> vertexCache;

				for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
				{
					uint32 triangleIndices[3];
					for (size_t j = 0; j < 3; j++)
					{
						int vIdx = faceIndices[j];
						int nIdx = normalIndices[j];

						std::pair<int, int> key = { vIdx, nIdx };
						auto it = vertexCache.find(key);

						if (it != vertexCache.end())
						{
							triangleIndices[j] = it->second;
						}
						else
						{
							FVector position = positions[vIdx];
							FVector normal = normals[nIdx];
							FVector4 color = Colors[vIdx];

							FVertexSimple vertexSimple{
								position.x, position.y, position.z, color.x, color.y, color.z, color.a,
								normal.x, normal.y, normal.z
							};

							uint32 newIndex = static_cast<uint32>(vertices.size());
							vertices.push_back(vertexSimple);
							vertexCache[key] = newIndex;
							triangleIndices[j] = newIndex;
						}
					}

					indices.push_back(triangleIndices[0]);
					indices.push_back(triangleIndices[1]);
					indices.push_back(triangleIndices[2]);
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

	if (!vertexBuffer || !indexBuffer) {
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

void FResourceMgr::RegisterMesh(FRenderer* renderer, const std::string& name, TArray<FVertexTexture>& vertices, uint32 vCount, TArray<uint32>& indices, uint32 iCount)
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

	ID3D11Buffer* vertexBuffer = renderer->CreateVertexTextureBuffer(vertices.data(), vCount * sizeof(FVertexTexture));
	ID3D11Buffer* indexBuffer = (!indices.empty() && iCount > 0) ? renderer->CreateIndexBuffer(indices, iCount * sizeof(UINT)) : nullptr;
	meshMap[name] = std::make_shared<FStaticMesh>(vertexBuffer, vCount, vertexArray, indexBuffer, iCount, indexArray);
}