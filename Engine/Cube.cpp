
#include "Cube.h"

#include "Renderer.h"

Cube::Cube()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	Renderer* renderer = new Renderer();
	renderer->LoadGeometry(box);
	AddComponent(renderer);
}
