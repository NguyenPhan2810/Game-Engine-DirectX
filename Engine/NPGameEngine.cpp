
#include "NPGameEngine.h"
#include "d3dx11effect.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"

NPGameEngine::NPGameEngine(HINSTANCE hInstance)
: D3DApp(hInstance)
, mVertexBuffer(nullptr)
, mIndexBuffer(nullptr)
, mFX(nullptr)
, mTech(nullptr)
, mfxWorldViewProj(nullptr)
, mInputLayout(nullptr)
, mWireframeRS(nullptr)
, mSolidRS(nullptr)
, mCamPhi(0.1f * MathHelper::Pi)
, mCamTheta(1.5f * MathHelper::Pi)
, mCamRadius(10.0f)
, mCamFOV(70.0f)
, mCamNear(1.0f)
, mCamFar(1000.0f)
, mMouseSensitivity(0.25f)
{
	mClientWidth = 1080;
	mClientHeight = 720;
	mMainWindowCaption = L"NP Game Engine v0.1";

	mLastMousePos = POINT{ 0 };

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}
}

NPGameEngine::~NPGameEngine()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);
	ReleaseCOM(mSolidRS);
}

bool NPGameEngine::Init()
{
	if (!D3DApp::Init())
		return false;

	InitRasterizerState();
	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexlayout();

	return true;
}

// Apply height and color to a vertex of a grid
void GetNewVector(const GeometryGenerator::Vertex& vert, Vertex& newVertex)
{
	auto x = vert.position.x;
	auto z = vert.position.z;
	auto y = vert.position.y;

	newVertex.Pos.x = x;
	newVertex.Pos.y = y;
	newVertex.Pos.z = z;
	newVertex.Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);;
}

void NPGameEngine::InitRasterizerState()
{
	D3D11_RASTERIZER_DESC wireframeRD, solidRD;

	ZeroMemory(&wireframeRD, sizeof(D3D11_RASTERIZER_DESC));
	ZeroMemory(&solidRD, sizeof(D3D11_RASTERIZER_DESC));

	solidRD.DepthClipEnable = TRUE;
	solidRD.CullMode = D3D11_CULL_BACK;
	solidRD.FillMode = D3D11_FILL_SOLID;

	wireframeRD.DepthClipEnable = TRUE;
	wireframeRD.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRD.CullMode = D3D11_CULL_BACK;

	HR(mDevice->CreateRasterizerState(&wireframeRD, &mWireframeRS));
	HR(mDevice->CreateRasterizerState(&solidRD, &mSolidRS));
}

void NPGameEngine::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateGeoSphere(0.5f, 3, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.indices.size();
	mGridIndexCount = grid.indices.size();
	mSphereIndexCount = sphere.indices.size();
	mCylinderIndexCount = cylinder.indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.vertices.size() +
		grid.vertices.size() +
		sphere.vertices.size() +
		cylinder.vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < box.vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.vertices[i].position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < grid.vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.vertices[i].position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < sphere.vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.vertices[i].position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < cylinder.vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.vertices[i].position;
		vertices[k].Color = black;
	}

	// Create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = vertices.size() * sizeof(Vertex);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = vertices.data();
	HR(mDevice->CreateBuffer(&vbd, &vertexInitData, &mVertexBuffer));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.indices.begin(), box.indices.end());
	indices.insert(indices.end(), grid.indices.begin(), grid.indices.end());
	indices.insert(indices.end(), sphere.indices.begin(), sphere.indices.end());
	indices.insert(indices.end(), cylinder.indices.begin(), cylinder.indices.end());

	// Create index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = totalIndexCount * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indices.data();

	HR(mDevice->CreateBuffer(&ibd, &indexInitData, &mIndexBuffer));
}

void NPGameEngine::BuildShaders()
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	HR(D3DX11CompileFromFile(L"FX/color.fx",
		0, 0, 0,
		"fx_5_0",
		shaderFlags, 0, 0,
		&compiledShader, &compilationMsgs,
		0));

	// Catch messages
	if (compilationMsgs)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, mDevice, &mFX));


	// Shader compilation done
	ReleaseCOM(compiledShader);

	if (mFX)
	{
		mTech = mFX->GetTechniqueByName("ColorTech");
		mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	}
}

void NPGameEngine::BuildVertexlayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(mDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void NPGameEngine::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	UpdateProjMatrix();
}

void NPGameEngine::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	// Only this window can capture the mouse input
	SetCapture(mhMainWnd);
}

void NPGameEngine::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void NPGameEngine::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(mMouseSensitivity * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(mMouseSensitivity * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mCamTheta -= dx;
		mCamPhi -= dy;

		// Restrict the angle mPhi.
		mCamPhi = MathHelper::Clamp(mCamPhi, 0.1f, MathHelper::Pi - 0.1f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void NPGameEngine::UpdateProjMatrix()
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(mCamFOV * MathHelper::Pi / 180.0f, AspectRatio(), mCamNear, mCamFar);
	XMStoreFloat4x4(&mProj, P);
}

void NPGameEngine::UpdateViewMatrix()
{
	// Convert Spherical to Cartesian coordinates.
	float x = mCamRadius * sinf(mCamPhi) * cosf(mCamTheta);
	float z = mCamRadius * sinf(mCamPhi) * sinf(mCamTheta);
	float y = mCamRadius * cosf(mCamPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void NPGameEngine::UpdateScene(float dt)
{
	UpdateViewMatrix();
}

void NPGameEngine::DrawScene()
{
	assert(mImmediateContext);
	assert(mSwapChain);

	// Clear buffer
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set up
	mImmediateContext->IASetInputLayout(mInputLayout);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set constants

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view * proj;

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		mImmediateContext->RSSetState(mWireframeRS);
		XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
		XMMATRIX worldViewProj = world * viewProj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);
		mImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		mImmediateContext->RSSetState(0);
		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		worldViewProj = world * viewProj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);
		mImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw center sphere.
		mImmediateContext->RSSetState(mWireframeRS);
		world = XMLoadFloat4x4(&mCenterSphere);
		worldViewProj = world * viewProj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);
		mImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);

		mImmediateContext->RSSetState(0);
		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			worldViewProj = world * viewProj;
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
			mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);
			mImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			worldViewProj = world * viewProj;
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
			mTech->GetPassByIndex(p)->Apply(0, mImmediateContext);
			mImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}
	}

	// Present buffer
	HR(mSwapChain->Present(0, 0));
}