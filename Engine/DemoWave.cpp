#include "DemoWave.h"

DemoWave::DemoWave(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 70;
}

DemoWave::~DemoWave()
{

}

void GetGridHeightColor(const GeometryGenerator::Vertex& vert, Vertex& newVertex)
{
	auto x = vert.position.x;
	auto z = vert.position.z;
	auto y = 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));

	newVertex.Pos.x = x;
	newVertex.Pos.y = y;
	newVertex.Pos.z = z;

	// Color the vertex based on its height.
	if (y < -10.0f)
	{
		// Sandy beach color.
		newVertex.Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
	}
	else if (y < 5.0f)
	{
		// Light yellow-green.
		newVertex.Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	}
	else if (y < 12.0f)
	{
		// Dark yellow-green.
		newVertex.Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
	}
	else if (y < 20.0f)
	{
		// Dark brown.
		newVertex.Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
	}
	else
	{
		// White snow.
		newVertex.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void DemoWave::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData skull;

	GeometryGenerator geoGen;
	geoGen.CreateGrid(200.0f, 200.0f, 50, 50, grid);
	geoGen.CreateFromFile(L"Models/skull.txt", skull);

	Vertex newVert;
	std::vector<Vertex> vertices;
	for (auto& vert : grid.vertices)
	{
		GetGridHeightColor(vert, newVert);
		vertices.push_back(newVert);
	}


	mGridObject = new BaseObject(mDevice, mImmediateContext);
	mGridObject->LoadGeometry(grid);
	mGridObject->CreateVertexBuffer(vertices);

	mCenterObject = new BaseObject(mDevice, mImmediateContext);
	mCenterObject->Translate(XMFLOAT3(0, 1, 0));
	mCenterObject->Scale(XMFLOAT3(2, 2, 2));
	mCenterObject->LoadGeometry(skull);
}
