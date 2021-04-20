#include "stdafx.h"
#include "DemoShowRoom.h"
#include "Cube.h"
#include "Renderer.h"

DemoShowRoom::DemoShowRoom(HINSTANCE hInstance)
: NPGameEngine(hInstance)
{
	
	mCamRadius = 13;
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

void DemoShowRoom::CreateTextures()
{
	mCrateTex = std::make_shared<Texture>(L"Textures/WoodCrate01.dds");
	mDarkBrickTex = std::make_shared<Texture>(L"Textures/darkbrickdxt1.dds");

	RENDERER(mFloorObj)->Texture = mDarkBrickTex.get();
	XMStoreFloat4x4(&RENDERER(mFloorObj)->TexTransform, XMMatrixScaling(3, 3, 0));
	
	RENDERER(mLWallObj)->Texture = mDarkBrickTex.get();
	XMStoreFloat4x4(&RENDERER(mLWallObj)->TexTransform, XMMatrixScaling(3, 3, 0));
	
	RENDERER(mRWallObj)->Texture = mDarkBrickTex.get();
	XMStoreFloat4x4(&RENDERER(mRWallObj)->TexTransform, XMMatrixScaling(3, 3, 0));

}

void DemoShowRoom::CreateObjects()
{
	mCenterObj = std::make_shared<Cube>();
	mFloorObj = std::make_shared<Cube>();
	mLWallObj = std::make_shared<Cube>();
	mRWallObj = std::make_shared<Cube>();


	mFloorObj->transform->Scale(XMFLOAT3(10, 0.1, 10));
	mFloorObj->transform->Translate(XMFLOAT3(0, -3, 0));

	mLWallObj->transform->Scale(XMFLOAT3(0.1, 10, 10));
	mLWallObj->transform->Translate(XMFLOAT3(-5, 5 - 3, 0));

	mRWallObj->transform->Scale(XMFLOAT3(10, 10, 0.1));
	mRWallObj->transform->Translate(XMFLOAT3(0, 5 - 3, 5));
}

void DemoShowRoom::CreateMaterials()
{
	auto& floorMat = RENDERER(mFloorObj)->GetMaterial();
	floorMat.Ambient = XMFLOAT4(0.1, 0.1, 0.1, 1);
	floorMat.Diffuse = XMFLOAT4(0.4, 0.4, 0.4, 1);
	floorMat.Specular = XMFLOAT4(0.3, 0.3, 0.3, 16);
}