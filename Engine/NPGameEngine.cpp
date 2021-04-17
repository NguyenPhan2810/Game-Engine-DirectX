#include "stdafx.h"

#include "NPGameEngine.h"
#include "d3dx11effect.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "GlobalDefinitions.h"
#include "Effects.h"

NPGameEngine::NPGameEngine(HINSTANCE hInstance)
: D3DApp(hInstance)
, mWireframeRS(nullptr)
, mSolidRS(nullptr)
, mCamPhi(0.1f * MathHelper::Pi)
, mCamTheta(1.5f * MathHelper::Pi)
, mCamRadius(200.0f)
, mCamFOV(70.0f)
, mCamNear(1.0f)
, mCamFar(1000.0f)
, mMouseSensitivity(0.25f)
, mEnableWireframe(false)
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
	ReleaseCOM(mWireframeRS);
	ReleaseCOM(mSolidRS);

	InputLayouts::DestroyAll();
	Effects::DestroyAll();
}

bool NPGameEngine::Init()
{
	if (!D3DApp::Init())
		return false;

	Effects::InitAll(mDevice);
	InputLayouts::InitAll(mDevice);

	InitRasterizerState();
	BuildGeometryBuffers();

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
	// Input
	if (GetAsyncKeyState('L') & 0x8000)
		mEnableWireframe = !mEnableWireframe;

	//===
	float x = mCamRadius * sinf(mCamPhi) * cosf(mCamTheta);
	float z = mCamRadius * sinf(mCamPhi) * sinf(mCamTheta);
	float y = mCamRadius * cosf(mCamPhi);

	mEyePosW = XMFLOAT3(x, y, z);
	UpdateViewMatrix();
}

void NPGameEngine::DrawScene()
{
	// Clear buffer
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set up
	mImmediateContext->IASetInputLayout(InputLayouts::PosNormal);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (mEnableWireframe)
		mImmediateContext->RSSetState(mWireframeRS);
	else
		mImmediateContext->RSSetState(mSolidRS);

	// Set constants

	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX viewProj = view * proj;
	auto allObjects = BaseObject::GetAllObjects();


	// Set per frame constants.
	Effects::BasicFX->SetDirLight(mDirLight);
	Effects::BasicFX->SetPointLight(mPointLight);
	Effects::BasicFX->SetSpotLight(mSpotLight);
	Effects::BasicFX->SetEyePosW(mEyePosW);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::BasicFX->Tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto obj : allObjects)
		{
			// Update constants
			XMMATRIX world = obj->LocalToWorldMatrix();
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * viewProj;
			const Material& material = obj->GetMaterial();

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(material);

			Effects::BasicFX->Tech->GetPassByIndex(p)->Apply(0, mImmediateContext);

			obj->Draw();
		}
	}

	// Present buffer
	HR(mSwapChain->Present(0, 0));
}