#include "DemoWave.h"

#define X 400
#define Z 400

DemoWave::DemoWave(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 70;

	// Directional light.
	mDirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mLandMat.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWaveMat.ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWaveMat.diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWaveMat.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
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
		v[i].Normal = mWaves.Normal(i);
	}

	mImmediateContext->Unmap(waveVertexBuffer, 0);
}

void DemoWave::DrawScene()
{
	// Clear buffer
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set up
	mImmediateContext->IASetInputLayout(mInputLayout);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//mImmediateContext->RSSetState(mWireframeRS);

	// Set constants

	mfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view * proj;
	XMMATRIX worldViewProj;
	auto allObjects = BaseObject::GetAllObjects();

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto obj : allObjects)
		{
			// Update constants
			worldViewProj = obj->LocalToWorldMatrix() * viewProj;
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
			mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);

			// Set state and draw
			if (obj->renderWireframe)
				mImmediateContext->RSSetState(mWireframeRS);
			else
				mImmediateContext->RSSetState(mSolidRS);

			obj->Draw();
		}
	}

	// Present buffer
	HR(mSwapChain->Present(0, 0));
}

void GetHillVertex(const GeometryGenerator::Vertex& vert, GLOBDEF::Vertex& newVertex)
{
	auto x = vert.position.x;
	auto z = vert.position.z;
	auto y = 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));

	newVertex.Pos.x = x;
	newVertex.Pos.y = y;
	newVertex.Pos.z = z;

	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));
	newVertex.Normal = n;
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
		GetHillVertex(vert, newVert);
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
	mCenterObject->Scale(XMFLOAT3(4, 4, 4));
	mCenterObject->LoadGeometry(skull);

	mWaveMesh = new BaseObject(mDevice, mImmediateContext);
	mWaveMesh->LoadGeometry(wave);

	D3D11_BUFFER_DESC waveVBD{ 0 };
	waveVBD.ByteWidth = mWaveMesh->GetVertexCount() * sizeof(GLOBDEF::Vertex);
	waveVBD.Usage = D3D11_USAGE_DYNAMIC;
	waveVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	waveVBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GeometryGenerator::ConvertToGlobVertex(wave.vertices, vertices);
	mWaveMesh->CreateVertexBuffer(vertices, waveVBD);
}
