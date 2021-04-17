#include "stdafx.h"
#include "DemoWave.h"

#define X 400
#define Z 400

float GetHillHeight(float x, float z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

void GetHillVertex(const GeometryGenerator::Vertex& vert, Vertex::PosNormal& newVertex)
{
	auto x = vert.position.x;
	auto z = vert.position.z;
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
}

DemoWave::DemoWave(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	mCamRadius = 70;

	mDirLight.Ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
	mDirLight.Diffuse = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.03f, 0.03f, 0.03f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.4f, -0.8f, 0.57735f);

	mPointLight.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mPointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.att = XMFLOAT3(1.0f, 0.1f, 0.0f);
	mPointLight.range = 2500.0f;
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
	if ((mTimer.TotalTime() - t_base) >= 0.01f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % (X - 10);
		DWORD j = 5 + rand() % (Z - 10);

		float r = MathHelper::RandF(1.0f, 2.5f);
		mWaves.Disturb(i, j, r);
	}

	mWaves.Update(dt);

	auto waveVertexBuffer = mWaveMesh->GetVertexBuffer();
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(mImmediateContext->Map(waveVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex::PosNormal* v = reinterpret_cast<Vertex::PosNormal*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);
	}

	mImmediateContext->Unmap(waveVertexBuffer, 0);


	mPointLight.Position.x = 40.0f * cosf(0.8f * mTimer.TotalTime());
	mPointLight.Position.z = 40.0f * sinf(0.8f * mTimer.TotalTime());
	mPointLight.Position.y = 20;
}

//void DemoWave::DrawScene()
//{
//	// Clear buffer
//	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
//	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	// Set up
//	mImmediateContext->IASetInputLayout(mInputLayout);
//	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	//mImmediateContext->RSSetState(mWireframeRS);
//
//	// Set constants
//
//	mfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));
//
//	XMMATRIX view = XMLoadFloat4x4(&mView);
//	XMMATRIX proj = XMLoadFloat4x4(&mProj);
//	XMMATRIX viewProj = view * proj;
//	XMMATRIX worldViewProj;
//	auto allObjects = BaseObject::GetAllObjects();
//
//	D3DX11_TECHNIQUE_DESC techDesc;
//	mTech->GetDesc(&techDesc);
//	for (UINT p = 0; p < techDesc.Passes; ++p)
//	{
//		for (auto obj : allObjects)
//		{
//			// Update constants
//			worldViewProj = obj->LocalToWorldMatrix() * viewProj;
//			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
//			mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);
//
//			// Set state and draw
//			if (obj->renderWireframe)
//				mImmediateContext->RSSetState(mWireframeRS);
//			else
//				mImmediateContext->RSSetState(mSolidRS);
//
//			obj->Draw();
//		}
//	}
//
//	// Present buffer
//	HR(mSwapChain->Present(0, 0));
//}

void DemoWave::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData wave;
	GeometryGenerator::MeshData skull;
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData geoSphere;


	GeometryGenerator geoGen;
	geoGen.CreateGrid(X, Z, 50, 50, grid);
	geoGen.CreateGrid(X, Z, X, Z, wave);
	geoGen.CreateBox(1, 1, 1, box);
	geoGen.CreateGeoSphere(1, 3, geoSphere);
	geoGen.CreateFromFile(L"Models/skull.txt", skull);

	Vertex::PosNormal newVert;
	std::vector<Vertex::PosNormal> vertices;
	for (auto& vert : grid.vertices)
	{
		GetHillVertex(vert, newVert);
		vertices.push_back(newVert);
	}


	mGridObject = new BaseObject(mDevice, mImmediateContext);
	mGridObject->LoadGeometry(grid); 
	D3D11_BUFFER_DESC gridVBD{ 0 };
	gridVBD.ByteWidth = mGridObject->GetVertexCount() * sizeof(Vertex::PosNormal);
	gridVBD.Usage = D3D11_USAGE_IMMUTABLE;
	gridVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	mGridObject->CreateVertexBuffer(vertices, gridVBD);

	mCenterObject = new BaseObject(mDevice, mImmediateContext);
	mCenterObject->Translate(XMFLOAT3(0, 10, 0));
	mCenterObject->Scale(XMFLOAT3(3.5, 3.5, 3.5));
	mCenterObject->LoadGeometry(skull);

	mWaveMesh = new BaseObject(mDevice, mImmediateContext);
	mWaveMesh->LoadGeometry(wave);

	D3D11_BUFFER_DESC waveVBD{ 0 };
	waveVBD.ByteWidth = mWaveMesh->GetVertexCount() * sizeof(Vertex::PosNormal);
	waveVBD.Usage = D3D11_USAGE_DYNAMIC;
	waveVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	waveVBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GeometryGenerator::ConvertToPosNormal(wave.vertices, vertices);
	mWaveMesh->CreateVertexBuffer(vertices, waveVBD);

	// Build mat
	auto& landMat = mGridObject->GetMaterial();
	landMat.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	landMat.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	landMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	auto& waveMat = mWaveMesh->GetMaterial();
	waveMat.ambient = XMFLOAT4(0.1f, 0.3f, 0.4f, 1.0f);
	waveMat.diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	waveMat.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 60.0f);

	auto& cubeMat = mCenterObject->GetMaterial();
	cubeMat.ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cubeMat.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cubeMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}
