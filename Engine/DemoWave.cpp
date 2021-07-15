
#include "DemoWave.h"
#include "Renderer.h"
#include "Cube.h"
#include "RenderStates.h"

#define X 160
#define Z 160

float GetHillHeight(float x, float z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

void GetHillVertex(const GeometryGenerator::Vertex& vert, Vertex::Basic32& newVertex)
{
	auto x = vert.Position.x;
	auto z = vert.Position.z;
	auto y = GetHillHeight(x, z);

	newVertex.Pos.x = x;
	newVertex.Pos.y = y;
	newVertex.Pos.z = z;

	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	newVertex.Normal = n;
	newVertex.Tex = vert.TexC;
}

DemoWave::DemoWave(HINSTANCE hInstance)
: NPGameEngine(hInstance)
, mCrateObject(nullptr)
, mWaveObject(nullptr)
, mGridObject(nullptr)
, mWaveTex(nullptr)
, mLandTex(nullptr)
{

	mCamRadius = 40;
}

DemoWave::~DemoWave()
{

}

bool DemoWave::Init()
{
	if (!NPGameEngine::Init())
		return false;


	BuildGeometryBuffers();
	mWaves.Init(X, Z, 1.0f, 0.03f, 10.25f, 0.2f);

	mLandTex = std::make_shared<Texture>(L"Textures/grass.dds");
	mCrateTex = std::make_shared<Texture>(L"Textures/WoodCrate02.dds");
	mFenceCrateTex = std::make_shared<Texture>(L"Textures/WireFence.dds");
	mWaveTex = std::make_shared<Texture>(L"Textures/water2.dds");

	RENDERER(mGridObject)->Texture = mLandTex.get();
	RENDERER(mWaveObject)->Texture = mWaveTex.get();
	RENDERER(mCrateObject)->Texture = mCrateTex.get();
	RENDERER(mFenceCrateObject)->Texture = mFenceCrateTex.get();

	// Tile/scale texture
	XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&RENDERER(mGridObject)->TexTransform, grassTexScale);

	return true;
}

void DemoWave::UpdateScene()
{
	NPGameEngine::UpdateScene();

	//
	// Every quarter second, generate a random wave.
	//
	static float t_base = 0.0f;
	if ((GameTimer::TotalTime() - t_base) >= 0.01f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % (X - 10);
		DWORD j = 5 + rand() % (X - 10);

		float r = MathHelper::RandF(1.0f, 2);
		mWaves.Disturb(i, j, r);
	}

	mWaves.Update(GameTimer::DeltaTime());

	auto waveVertexBuffer = RENDERER(mWaveObject)->GetVertexBuffer();
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(mImmediateContext->Map(waveVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex::Basic32* v = reinterpret_cast<Vertex::Basic32*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);

		// Derive tex-coords in [0,1] from position.
		v[i].Tex.x = 0.5f + mWaves[i].x / mWaves.Width();
		v[i].Tex.y = 0.5f - mWaves[i].z / mWaves.Depth();
	}

	mImmediateContext->Unmap(waveVertexBuffer, 0);
}

void DemoWave::FixedUpdateScene()
{
	// Scroll water texture
	mWaveTexOffset.x += 0.05f * GameTimer::FixedDeltaTime();
	mWaveTexOffset.y += 0.01f * GameTimer::FixedDeltaTime();

	XMMATRIX waveTexScale = XMMatrixScaling(10.0f, 10.0f, 0.0f);
	XMMATRIX waveTexOffset = XMMatrixTranslation(mWaveTexOffset.x, mWaveTexOffset.y, 0.0f);
	XMStoreFloat4x4(&RENDERER(mWaveObject)->TexTransform, waveTexScale * waveTexOffset);
}

void DemoWave::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData wave;
//	GeometryGenerator::MeshData skull;
	GeometryGenerator::MeshData box;
//	GeometryGenerator::MeshData geoSphere;


	GeometryGenerator geoGen;
	geoGen.CreateGrid(X, Z, 50, 50, grid);
	geoGen.CreateGrid(X, Z, X, Z, wave);
	geoGen.CreateBox(1, 1, 1, box);
//	geoGen.CreateGeoSphere(1, 3, geoSphere);
//	geoGen.CreateFromFile(L"Models/skull.txt", skull);


	//=======================================
	// Hill
	Vertex::Basic32 newVert;
	std::vector<Vertex::Basic32> Vertices;
	for (auto& vert : grid.Vertices)
	{
		GetHillVertex(vert, newVert);
		Vertices.push_back(newVert);
	}

	mGridObject = std::make_shared<Cube>();
	RENDERER(mGridObject)->LoadGeometry(grid); 

	// Set hills height
	D3D11_BUFFER_DESC gridVBD{ 0 };
	gridVBD.ByteWidth = RENDERER(mGridObject)->GetVertexCount() * sizeof(Vertex::Basic32);
	gridVBD.Usage = D3D11_USAGE_IMMUTABLE;
	gridVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	RENDERER(mGridObject)->CreateVertexBuffer(Vertices, gridVBD);


	//=======================================
	// Waves

	mWaveObject = std::make_shared<Cube>();
	RENDERER(mWaveObject)->LoadGeometry(wave);

	D3D11_BUFFER_DESC waveVBD{ 0 };
	waveVBD.ByteWidth = RENDERER(mWaveObject)->GetVertexCount() * sizeof(Vertex::Basic32);
	waveVBD.Usage = D3D11_USAGE_DYNAMIC;
	waveVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	waveVBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GeometryGenerator::ConvertToBasic32(wave.Vertices, Vertices);
	RENDERER(mWaveObject)->CreateVertexBuffer(Vertices, waveVBD);

	//=======================================
	// Crate
	mCrateObject = std::make_shared<Cube>();
	mCrateObject->transform->Scale(XMFLOAT3(10, 10, 10));
	mCrateObject->transform->Rotate(XMFLOAT3(1, 0, 0), XM_PI / 8);
	mCrateObject->transform->Rotate(XMFLOAT3(0, 0, 1), XM_PI / 9);
	mCrateObject->transform->Translate(XMFLOAT3(0, 0.5, 0));

	//=======================================
	// Fence crate
	mFenceCrateObject = std::make_shared<Cube>();
	mFenceCrateObject->transform->Scale(XMFLOAT3(10, 10, 10));
	mFenceCrateObject->transform->Rotate(XMFLOAT3(1, 0, 0), XM_PI / 10);
	mFenceCrateObject->transform->Rotate(XMFLOAT3(0, 0, 1), XM_PI / 10);
	mFenceCrateObject->transform->Translate(XMFLOAT3(-10, 0.2, 10));

	// Build mat
	auto& landMat = RENDERER(mGridObject)->GetMaterial();
	landMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	landMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	landMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	auto& waveMat = RENDERER(mWaveObject)->GetMaterial();
	waveMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	waveMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.4f);
	waveMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	auto& crateMat = RENDERER(mCrateObject)->GetMaterial();
	crateMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	crateMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	crateMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	// Build blend
	RENDERER(mWaveObject)->BlendState = RenderStates::TransparentBS;
	RENDERER(mWaveObject)->RasterizerState = RenderStates::NoCullRS;;
	RENDERER(mFenceCrateObject)->RasterizerState = RenderStates::NoCullRS;;
}
