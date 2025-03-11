#include "ResourceMgr.h"
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


	numVerticesGizmo = sizeof(gizmoVertices) / sizeof(FVertexSimple);
	vertexBufferGizmo = renderer->CreateVertexBuffer(gizmoVertices, sizeof(gizmoVertices));

	numVerticesArrowX = sizeof(arrow_x_vertices) / sizeof(FVertexSimple);
	vertexBufferArrowX = renderer->CreateVertexBuffer(arrow_x_vertices, sizeof(arrow_x_vertices));


	numVerticesArrowY = sizeof(arrow_y_vertices) / sizeof(FVertexSimple);
	vertexBufferArrowY = renderer->CreateVertexBuffer(arrow_y_vertices, sizeof(arrow_y_vertices));

	numVerticesArrowZ = sizeof(arrow_z_vertices) / sizeof(FVertexSimple);
	vertexBufferArrowZ = renderer->CreateVertexBuffer(arrow_z_vertices, sizeof(arrow_z_vertices));
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
