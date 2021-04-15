
#include "NPGameEngine.h"
#include "d3dx11effect.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "GlobalDefinitions.h"
#include "LightHelper.h"

NPGameEngine::NPGameEngine(HINSTANCE hInstance)
: D3DApp(hInstance)
, mFX(nullptr)
, mTech(nullptr)
, mfxWorldViewProj(nullptr)
, mInputLayout(nullptr)
, mWireframeRS(nullptr)
, mSolidRS(nullptr)
, mCamPhi(0.1f * MathHelper::Pi)
, mCamTheta(1.5f * MathHelper::Pi)
, mCamRadius(100.0f)
, mCamFOV(45.0f)
, mCamNear(1.0f)
, mCamFar(1000.0f)
, mMouseSensitivity(0.25f)
{
	mClientWidth = 1080;
	mClientHeight = 720;
	mMainWindowCaption = L"NP Game Engine";

	mLastMousePos = POINT{ 0 };

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}

NPGameEngine::~NPGameEngine()
{
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
	wireframeRD.CullMode = D3D11_CULL_NONE;

	HR(mDevice->CreateRasterizerState(&wireframeRD, &mWireframeRS));
	HR(mDevice->CreateRasterizerState(&solidRD, &mSolidRS));
}

void NPGameEngine::BuildGeometryBuffers()
{
	
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

	HR(D3DX11CompileFromFile(L"FX/Lighting.fx",
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
		mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
		mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
		mfxEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
		mfxDirLight = mFX->GetVariableByName("gDirLight");
		mfxMaterial = mFX->GetVariableByName("gMaterial");
	}
}

void NPGameEngine::BuildVertexlayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
	// Clear buffer
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set up
	mImmediateContext->IASetInputLayout(mInputLayout);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//mImmediateContext->RSSetState(mWireframeRS);

	// Set constants

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view * proj;
	auto allObjects = BaseObject::GetAllObjects();


	// Set per frame constants.
	mfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mfxDirLmDirLightight));
	//mfxPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));
	//mfxSpotLight->SetRawValue(&mSpotLight, 0, sizeof(mSpotLight));
	mfxEyePosW->SetRawValue(&mfxEyePosW, 0, sizeof(mfxEyePosW));

	Material tempMat;
	tempMat.ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	tempMat.diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	tempMat.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto obj : allObjects)
		{
			// Update constants
			XMMATRIX world = obj->LocalToWorldMatrix();
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * viewProj;

			mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
			mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
			mfxMaterial->SetRawValue(&tempMat, 0, sizeof(tempMat));

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