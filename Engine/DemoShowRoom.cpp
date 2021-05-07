#include "stdafx.h"
#include "DemoShowRoom.h"
#include "Cube.h"
#include "Plane.h"
#include "Renderer.h"
#include "Effects.h"
#include "MathHelper.h"
#include "RenderStates.h"

DemoShowRoom::DemoShowRoom(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	
	mCamRadius = 11;
}

DemoShowRoom::~DemoShowRoom()
{

}

bool DemoShowRoom::Init()
{
	if (!NPGameEngine::Init())
		return false;

	CreateObjects();
	CreateTextures();
	CreateMaterials();

	return true;
}

void DemoShowRoom::UpdateScene()
{
	NPGameEngine::UpdateScene();

}

void DemoShowRoom::DrawScene()
{
	// Clear buffer
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set up
	mImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX viewProj = view * proj;
	auto allObjects = BaseObject::GetAllObjects();


	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mEyePosW);
	//Effects::BasicFX->SetFogColor(Colors::Silver);
	//Effects::BasicFX->SetFogStart(15.0f);
	//Effects::BasicFX->SetFogRange(70.0f);

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

	// Render blend objects later
	std::vector<BaseObject*> blendedObjects;
	std::vector<BaseObject*> nonBlendedObjects;
	// Render mirror
	for (auto& obj : allObjects)
	{
		if (!obj->enabled)
			continue;

		auto renderer = obj->GetComponent<Renderer>("Renderer");
		if (renderer)
		{
			if (renderer->BlendState)
				blendedObjects.emplace_back(obj);
			else
				nonBlendedObjects.emplace_back(obj);
		}
	}

#pragma region Non blended objects

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto& obj : nonBlendedObjects)
		{
			// Update per object constants
			XMMATRIX world = obj->transform->LocalToWorldMatrix();
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * viewProj;

			auto renderer = RENDERER(obj);
			const Material& material = renderer->GetMaterial();

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(material);

			auto& texture = renderer->Texture;
			if (texture)
			{
				Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&renderer->TexTransform));
				Effects::BasicFX->SetDiffuseMap(texture->GetDiffuseMapSRV());
				Effects::BasicFX->SetUseTexture(mUseTexture);
			}

			mImmediateContext->RSSetState(renderer->RasterizerState);
			activeTech->GetPassByIndex(p)->Apply(0, mImmediateContext);

			obj->Draw();

			// Restore default render state.
			mImmediateContext->RSSetState(0);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetDiffuseMap(0);
			Effects::BasicFX->SetUseTexture(0);
		}
	}

#pragma endregion

#pragma region Blended objects

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto& obj : blendedObjects)
		{
			// Update per object constants
			XMMATRIX world = obj->transform->LocalToWorldMatrix();
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * viewProj;

			auto renderer = RENDERER(obj);
			const Material& material = renderer->GetMaterial();

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(material);

			auto& texture = renderer->Texture;
			if (texture)
			{
				Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&renderer->TexTransform));
				Effects::BasicFX->SetDiffuseMap(texture->GetDiffuseMapSRV());
				Effects::BasicFX->SetUseTexture(mUseTexture);
			}

			mImmediateContext->RSSetState(renderer->RasterizerState);
			mImmediateContext->OMSetBlendState(renderer->BlendState, blendFactor, 0xfffffff);
			activeTech->GetPassByIndex(p)->Apply(0, mImmediateContext);

			obj->Draw();

			// Restore default render state.
			mImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
			mImmediateContext->RSSetState(0);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetDiffuseMap(0);
			Effects::BasicFX->SetUseTexture(0);
		}
	}

#pragma endregion

#pragma region Mirror
	// Get all objects will be reflected
	// Calculate dot product of vector d = object's pos - mirror pos and the normal of mirror
	// If the dot product is positive then the object will be reflected
	std::vector<BaseObject*> reflectedObjects;
	XMFLOAT3 mirrorPosFloat = mMirrorObj->transform->GetWorldPosition();
	XMVECTOR mirrorPos = XMLoadFloat3(&mirrorPosFloat);
	XMFLOAT3 mirrorNormalFloat = mMirrorObj->transform->GetWorldY();
	XMVECTOR mirrorNormal = XMLoadFloat3(&mirrorNormalFloat);
	
	for (auto& obj : allObjects)
	{
		if (!obj->enabled)
			continue;

		XMFLOAT3 objectPosFloat = obj->transform->GetWorldPosition();
		XMVECTOR objectPos = XMLoadFloat3(&objectPosFloat);

		auto d = objectPos - mirrorPos;

		float dot = XMVectorGetX(XMVector3Dot(mirrorNormal, d));

		if (dot > 0)
			reflectedObjects.push_back(obj);
	}


	// Apply reflection through mirror plane
	XMMATRIX R = XMMatrixReflect(mirrorNormal);

	// Reflect the light direction first
	XMFLOAT3 oldLightDirections[3];
	for (int i = 0; i < 3; ++i)
	{
		oldLightDirections[i] = mDirLights[i].Direction;
		XMVECTOR lightDir = XMLoadFloat3(&mDirLights[i].Direction);
		XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&mDirLights[i].Direction, reflectedLightDir);
	}
	Effects::BasicFX->SetDirLights(mDirLights);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (auto& obj : reflectedObjects)
		{
			XMMATRIX invTranslateMatrix = XMMatrixTranslation(
				-mirrorPosFloat.x,
				-mirrorPosFloat.y,
				-mirrorPosFloat.z);
			XMMATRIX translateMatrix = XMMatrixTranslation(
				mirrorPosFloat.x,
				mirrorPosFloat.y,
				mirrorPosFloat.z);

			XMMATRIX world = obj->transform->LocalToWorldMatrix() * invTranslateMatrix * R * translateMatrix;
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * viewProj;

			auto renderer = RENDERER(obj);
			const Material& material = renderer->GetMaterial();

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(material);

			auto& texture = renderer->Texture;
			if (texture)
			{
				Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&renderer->TexTransform));
				Effects::BasicFX->SetDiffuseMap(texture->GetDiffuseMapSRV());
				Effects::BasicFX->SetUseTexture(mUseTexture);
			}

			mImmediateContext->RSSetState(RenderStates::CullClockwiseRS);
			activeTech->GetPassByIndex(p)->Apply(0, mImmediateContext);

			obj->Draw();

			// Restore default render state.
			mImmediateContext->RSSetState(0);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetDiffuseMap(0);
			Effects::BasicFX->SetUseTexture(0);
		}
	}

	// Restore lights direction
	for (int i = 0; i < 3; ++i)
	{
		mDirLights[i].Direction = oldLightDirections[i];
	}
	Effects::BasicFX->SetDirLights(mDirLights);

#pragma endregion


	// Present buffer
	HR(mSwapChain->Present(0, 0));
}

void DemoShowRoom::CreateObjects()
{
	GeometryGenerator::MeshData skullMesh;
	GeometryGenerator::CreateFromFile(L"Models/skull.txt", skullMesh);

	mCenterObj = std::make_shared<Cube>();
	mFloorObj = std::make_shared<Cube>();
	mLWallObj = std::make_shared<Cube>();
	mRWallObj = std::make_shared<Cube>();
	mMirrorObj = std::make_shared<Plane>();

	RENDERER(mCenterObj)->LoadGeometry(skullMesh);
	mCenterObj->transform->Scale(XMFLOAT3(0.6, 0.6, 0.6));

	mFloorObj->transform->Scale(XMFLOAT3(10, 0.1, 10));
	mFloorObj->transform->Translate(XMFLOAT3(0, -1, 0));

	mLWallObj->transform->Scale(XMFLOAT3(0.1, 10, 10));
	mLWallObj->transform->Translate(XMFLOAT3(-5, 5 - 1, 0));

	mRWallObj->transform->Scale(XMFLOAT3(10, 10, 0.1));
	mRWallObj->transform->Translate(XMFLOAT3(0, 5 - 1, 5));

	//mMirrorObj->enabled = false;
	mMirrorObj->transform->Scale(XMFLOAT3(3, 1, 4));
	mMirrorObj->transform->Rotate(XMFLOAT3(0, 0, 1), -MathHelper::Pi / 2);
	mMirrorObj->transform->Translate(XMFLOAT3(-5 + 0.1, 3, 0));
}

void DemoShowRoom::CreateTextures()
{
	mCrateTex = std::make_shared<Texture>(L"Textures/WoodCrate01.dds");
	mDarkBrickTex = std::make_shared<Texture>(L"Textures/darkbrickdxt1.dds");
	mCheckBoardTex = std::make_shared<Texture>(L"Textures/checkboard.dds");
	mMirrorTex = std::make_shared<Texture>(L"Textures/ice.dds");

	RENDERER(mFloorObj)->Texture = mCheckBoardTex.get();
	XMStoreFloat4x4(&RENDERER(mFloorObj)->TexTransform, XMMatrixScaling(8, 8, 0));
	
	RENDERER(mLWallObj)->Texture = mDarkBrickTex.get();
	XMStoreFloat4x4(&RENDERER(mLWallObj)->TexTransform, XMMatrixScaling(3, 3, 0));
	
	RENDERER(mRWallObj)->Texture = mDarkBrickTex.get();
	XMStoreFloat4x4(&RENDERER(mRWallObj)->TexTransform, XMMatrixScaling(3, 3, 0));

	RENDERER(mMirrorObj)->Texture = mMirrorTex.get();

}

void DemoShowRoom::CreateMaterials()
{
	auto& floorMat = RENDERER(mFloorObj)->GetMaterial();
	floorMat.Ambient = XMFLOAT4(0.2, 0.2, 0.2, 1);
	floorMat.Diffuse = XMFLOAT4(0.5, 0.5, 0.5, 1);
	floorMat.Specular = XMFLOAT4(0.7, 0.7, 0.7, 96);

	auto& skullMat = RENDERER(mCenterObj)->GetMaterial();
	skullMat.Ambient = XMFLOAT4(0.4, 0.42, 0.37, 1);
	skullMat.Diffuse = XMFLOAT4(0.65, 0.6, 0.45, 1);
	skullMat.Specular = XMFLOAT4(0.3, 0.3, 0.3, 16);
}