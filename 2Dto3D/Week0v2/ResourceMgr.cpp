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
	numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
	vertexBufferSphere = renderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

	numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
	vertexBufferCube = renderer->CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));

	numIndicesCube = sizeof(cube_indices) / sizeof(uint32);
	indiceBufferCube = renderer->CreateIndexBuffer(cube_indices, sizeof(cube_indices));


	numVerticesGizmo = sizeof(gizmoVertices) / sizeof(FVertexSimple);
	vertexBufferGizmo = renderer->CreateVertexBuffer(gizmoVertices, sizeof(gizmoVertices));

	//numVerticesArrowX = sizeof(translationX_vertices) / sizeof(FVertexSimple);
	//vertexBufferArrowX = renderer->CreateVertexBuffer(translationX_vertices, sizeof(translationX_vertices));
	LoadObjAsset(renderer, L"Assets/AxisArrowX.obj", &vertexBufferArrowX, numVerticesArrowX, &indiceBufferArrowX, numIndicesArrowX);
	if (vertexBufferArrowX)
	{
		UE_LOG(LogLevel::Error, "XXXXXXXXXX");
	}
	//numVerticesArrowY = sizeof(translationY_vertices) / sizeof(FVertexSimple);
	//vertexBufferArrowY = renderer->CreateVertexBuffer(translationY_vertices, sizeof(translationY_vertices));

	LoadObjAsset(renderer, L"Assets/AxisArrowY.obj", &vertexBufferArrowY, numVerticesArrowY, &indiceBufferArrowY, numIndicesArrowY);
	if (vertexBufferArrowY)
	{
		UE_LOG(LogLevel::Error, "YYYYYYYYYY");
	}
	//numVerticesArrowZ = sizeof(translationZ_vertices) / sizeof(FVertexSimple);
	//vertexBufferArrowZ = renderer->CreateVertexBuffer(translationZ_vertices, sizeof(translationZ_vertices));
	LoadObjAsset(renderer, L"Assets/AxisArrowZ.obj", &vertexBufferArrowZ, numVerticesArrowZ, &indiceBufferArrowZ, numIndicesArrowZ);
	if (vertexBufferArrowZ)
	{
		UE_LOG(LogLevel::Error, "ZZZZZZZZZ");
	}
}

void FResourceMgr::Release(FRenderer* renderer)
{
	renderer->ReleaseVertexBuffer(vertexBufferSphere);
	renderer->ReleaseVertexBuffer(vertexBufferCube);
	renderer->ReleaseVertexBuffer(vertexBufferGizmo);
	renderer->ReleaseVertexBuffer(vertexBufferArrowX);
	renderer->ReleaseVertexBuffer(vertexBufferArrowY);
	renderer->ReleaseVertexBuffer(vertexBufferArrowZ);

}



void FResourceMgr::LoadObjAsset(FRenderer* renderer, FWString filepath, ID3D11Buffer** vertexBuffer, UINT& numVertices, ID3D11Buffer** indexBuffer, UINT& numIndices)
{
	std::ifstream objFile(filepath.c_str());
	//FVector color = AxisColors[axis];

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
	UE_LOG(LogLevel::Error, "Arrow Vertex Size : %d", vertices.size());
	UE_LOG(LogLevel::Error, "Arrow Vertex Size : %d", indices.size());
	*vertexBuffer = renderer->CreateVertexBuffer(vertices, vertices.size() * sizeof(FVertexSimple));
	*indexBuffer = renderer->CreateIndexBuffer(indices, indices.size() * sizeof(uint32));
	numVertices = static_cast<UINT>(vertices.size());
	numIndices = static_cast<UINT>(indices.size());


}


