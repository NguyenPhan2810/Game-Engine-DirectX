#include "DemoWave.h"

#define X 400
#define Z 400

DemoWave::DemoWave(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 70;

}

DemoWave::~DemoWave()
{

}

bool DemoWave::Init()
{
	if (!NPGameEngine::Init())
		return false;

	mWaves.Init(X, Z, 1.0f, 0.03f, 10.25f, 0.2f);


	return true;
}

void DemoWave::UpdateScene(float dt)
{
	NPGameEngine::UpdateScene(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//
	
	//
	// Every quarter second, generate a random wave.
	//
	static float t_base = 0.0f;
	if ((mTimer.TotalTime() - t_base) >= 0.05f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % (X - 5);
		DWORD j = 5 + rand() % (Z - 5);

		float r = MathHelper::RandF(5.0f, 10.0f);

		mWaves.Disturb(i, j, r);
	}

	mWaves.Update(dt);

	auto waveVertexBuffer = mWaveMesh->GetVertexBuffer();
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(mImmediateContext->Map(waveVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	GLOBDEF::Vertex* v = reinterpret_cast<GLOBDEF::Vertex*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	mImmediateContext->Unmap(waveVertexBuffer, 0);
}

void GetGridHeightColor(const GeometryGenerator::Vertex& vert, GLOBDEF::Vertex& newVertex)
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
	GeometryGenerator::MeshData wave;
	GeometryGenerator::MeshData skull;


	GeometryGenerator geoGen;
	geoGen.CreateGrid(X, Z, 50, 50, grid);
	geoGen.CreateGrid(X, Z, X, Z, wave);
	geoGen.CreateFromFile(L"Models/skull.txt", skull);

	GLOBDEF::Vertex newVert;
	std::vector<GLOBDEF::Vertex> vertices;
	for (auto& vert : grid.vertices)
	{
		GetGridHeightColor(vert, newVert);
		vertices.push_back(newVert);
	}


	mGridObject = new BaseObject(mDevice, mImmediateContext);
	mGridObject->LoadGeometry(grid); 
	D3D11_BUFFER_DESC gridVBD{ 0 };
	gridVBD.ByteWidth = mGridObject->GetVertexCount() * sizeof(GLOBDEF::Vertex);
	gridVBD.Usage = D3D11_USAGE_IMMUTABLE;
	gridVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	mGridObject->CreateVertexBuffer(vertices, gridVBD);

	mCenterObject = new BaseObject(mDevice, mImmediateContext);
	mCenterObject->Translate(XMFLOAT3(0, 1, 0));
	mCenterObject->Scale(XMFLOAT3(2, 2, 2));
	mCenterObject->LoadGeometry(skull);

	mWaveMesh = new BaseObject(mDevice, mImmediateContext);
	mWaveMesh->renderWireframe = true;
	mWaveMesh->LoadGeometry(wave);

	D3D11_BUFFER_DESC waveVBD{ 0 };
	waveVBD.ByteWidth = mWaveMesh->GetVertexCount() * sizeof(GLOBDEF::Vertex);
	waveVBD.Usage = D3D11_USAGE_DYNAMIC;
	waveVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	waveVBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GeometryGenerator::ConvertToGlobVertex(wave.vertices, vertices);
	mWaveMesh->CreateVertexBuffer(vertices, waveVBD);
}
