
#include "Plane.h"

#include "Renderer.h"

Plane::Plane()
{
	GeometryGenerator::MeshData plane;
	GeometryGenerator geoGen;
	geoGen.CreateGrid(2, 2, 2, 2, plane);
	Renderer* renderer = new Renderer();
	renderer->LoadGeometry(plane);
	AddComponent(renderer);
}
