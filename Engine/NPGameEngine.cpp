#include "stdafx.h"

#include "NPGameEngine.h"
#include "d3dx11effect.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "GlobalDefinitions.h"
#include "Effects.h"
#include "Renderer.h"

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
, mLightCount(3)
, mUseTexture(true)
{
	mClientWidth = 1080;
	mClientHeight = 720;
	mMainWindowCaption = L"NP Game Engine";

	mLastMousePos = POINT{ 0 };

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);
}

NPGameEngine::~NPGameEngine()
{
	ReleaseCOM(mWireframeRS);
	ReleaseCOM(mSolidRS);

	InputLayouts::DestroyAll();
	Effects::DestroyAll();
}

void NPGameEngine::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	UpdateProjMatrix();
}

bool NPGameEngine::Init()
{
	if (!D3DApp::Init())
		return false;

	Effects::InitAll(mDevice);
	InputLayouts::InitAll(mDevice);

	InitRasterizerState();

	for (auto obj : BaseObject::GetAllObjects())
		obj->Init();

	return true;
}

void NPGameEngine::UpdateScene()
{
	// Input
	mEnableWireframe = GetAsyncKeyState('L') & 0x8000;
	if (GetAsyncKeyState('T') & 0x8000)
		mUseTexture = !mUseTexture;
	if (GetAsyncKeyState('0') & 0x8000)
		mLightCount = 0;
	if (GetAsyncKeyState('1') & 0x8000)
		mLightCount = 1;
	if (GetAsyncKeyState('2') & 0x8000)
		mLightCount = 2;
	if (GetAsyncKeyState('3') & 0x8000)
		mLightCount = 3;
		

	//===
	float x = mCamRadius * sinf(mCamPhi) * cosf(mCamTheta);
	float z = mCamRadius * sinf(mCamPhi) * sinf(mCamTheta);
	float y = mCamRadius * cosf(mCamPhi);

	mEyePosW = XMFLOAT3(x, y, z);
	UpdateViewMatrix();

	for (auto obj : BaseObject::GetAllObjects())
		obj->Update();
}

void NPGameEngine::FixedUpdateScene()
{
	for (auto obj : BaseObject::GetAllObjects())
		obj->FixedUpdate();
}

void NPGameEngine::DrawScene()
{
	// Clear buffer
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set up
	mImmediateContext->IASetInputLayout(InputLayouts::Basic32);
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
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mEyePosW);

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3Tech;
	switch (mLightCount)
	{
	case 0: activeTech = Effects::BasicFX->Light0Tech; break;
	case 1: activeTech = Effects::BasicFX->Light1Tech; break;
	case 2: activeTech = Effects::BasicFX->Light2Tech; break;
	case 3: activeTech = Effects::BasicFX->Light3Tech; break;
	}

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto& obj : allObjects)
		{
			auto renderer = (Renderer*)(obj->GetComponentByName("Renderer"));
			if (renderer)
			{
				// Update constants
				XMMATRIX world = obj->transform->LocalToWorldMatrix();
				XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
				XMMATRIX worldViewProj = world * viewProj;
				const Material& material = renderer->GetMaterial();

				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetMaterial(material);

				auto& texture = renderer->Texture;
				if (texture)
				{
					Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&texture->TexTransform));
					Effects::BasicFX->SetDiffuseMap(texture->GetDiffuseMapSRV());
					Effects::BasicFX->SetUseTexture(mUseTexture);
				}
				else
				{
					Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
					Effects::BasicFX->SetDiffuseMap(0);
					Effects::BasicFX->SetUseTexture(0);
				}

				activeTech->GetPassByIndex(p)->Apply(0, mImmediateContext);

				obj->Draw();
			}
		}
	}

	// Present buffer
	HR(mSwapChain->Present(0, 0));
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
